/* zebra daemon main routine.
 * Copyright (C) 1997, 98 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  
 */

#include "platform.h"
#include "version.h"
#include "command.h"
#include "thread.h"
#include "filter.h"
#include "memory.h"
#include "prefix.h"
#include "log.h"
#include "privs.h"
#include "sigevent.h"
#include "vrf.h"
#include "rib.h"
#include "zserv.h"
#include "debug.h"
#include "router-id.h"
#include "irdp.h"
#include "rtadv.h"
#include "zebra_fpm.h"
#include "common.h"
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else 
extern void exit();
#endif
#include "uart.h"
#include <errno.h>
//根据任务列表创建任务定时器
void create_task_timer(void)
{
  struct sigevent sev;
  struct itimerspec its;
  timer_t timerid;

  /* Create the timer */
  sev.sigev_notify = SIGEV_THREAD;
  sev.sigev_value.sival_ptr = &timerid;
  sev.sigev_notify_function = uart4_recv;
  sev.sigev_notify_attributes = NULL;

  if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
      printf("timer_create\r\n");
    
  /* Start the timer */
  its.it_value.tv_sec = 1;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 1;
  its.it_interval.tv_nsec = 0;
  if (timer_settime(timerid, 0, &its, NULL) == -1)
      printf("timer_settime\r\n");

}




/* Zebra instance */
struct platform_t platformd =
{
  .rtm_table_default = 0,
};

/* process id. */
pid_t pid;

/* Pacify zclient.o in libzebra, which expects this variable. */
struct thread_master *master;

/* Route retain mode flag. */
int retain_mode = 0;

/* Don't delete kernel route. */
int keep_kernel_mode = 0;

#ifdef HAVE_NETLINK
/* Receive buffer size for netlink socket */
u_int32_t nl_rcvbufsize = 0;
#endif /* HAVE_NETLINK */

platform_capabilities_t _caps_p [] = 
{
  ZCAP_NET_ADMIN,
  ZCAP_SYS_ADMIN,
  ZCAP_NET_RAW,
};

/* zebra privileges to run with */
struct platform_privs_t zserv_privs =
{
  .caps_p = _caps_p,
  .cap_num_p = array_size(_caps_p),
  .cap_num_i = 0
};

/* Default configuration file path. */
char config_default[] = SYSCONFDIR DEFAULT_CONFIG_FILE;

/* Process ID saved for use by init system */
const char *pid_file = PATH_PLATFORM_PID;

/* Help information display. */
static void
usage (char *progname, int status)
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n", progname);
  else
    {    
      printf ("Usage : %s [OPTION...]\n\n"\
	      "Daemon which manages kernel routing table management and "\
	      "redistribution between different routing protocols.\n\n"\
	      "-b, --batch        Runs in batch mode\n"\
	      "-d, --daemon       Runs in daemon mode\n"\
	      "-f, --config_file  Set configuration file name\n"\
	      "-F, --fpm_format   Set fpm format to 'netlink' or 'protobuf'\n"\
	      "-i, --pid_file     Set process identifier file name\n"\
	      "-z, --socket       Set path of zebra socket\n"\
	      "-k, --keep_kernel  Don't delete old routes which installed by "\
				  "zebra.\n"\
	      "-C, --dryrun       Check configuration for validity and exit\n"\
	      "-A, --vty_addr     Set vty's bind address\n"\
	      "-P, --vty_port     Set vty's port number\n"\
	      "-r, --retain       When program terminates, retain added route "\
				  "by zebra.\n"\
	      "-u, --user         User to run as\n"\
	      "-g, --group	  Group to run as\n", progname);
#ifdef HAVE_NETLINK
      printf ("-s, --nl-bufsize   Set netlink receive buffer size\n");
#endif /* HAVE_NETLINK */
      printf ("-v, --version      Print program version\n"\
	      "-h, --help         Display this help and exit\n"\
	      "\n"\
	      "Report bugs to %s\n", BUG_ADDRESS);
    }

  exit (status);
}

/* SIGHUP handler. */
static void 
sighup (void)
{
  zlog_info ("SIGHUP received");

  /* Reload of config file. */
  ;
}

/* SIGINT handler. */
static void
sigint (void)
{
  zlog_notice ("Terminating on signal");

  if (!retain_mode)
    rib_close ();

  exit (0);
}

/* SIGUSR1 handler. */
static void
sigusr1 (void)
{
  zlog_rotate (NULL);
}

struct signal_t platform_signals[] =
{
  { 
    .signal = SIGHUP, 
    .handler = &sighup,
  },
  {
    .signal = SIGUSR1,
    .handler = &sigusr1,
  },
  {
    .signal = SIGINT,
    .handler = &sigint,
  },
  {
    .signal = SIGTERM,
    .handler = &sigint,
  },
};

/* Callback upon creating a new VRF. */
static int
platform_vrf_new (vrf_id_t vrf_id, void **info)
{
  struct platform_vrf *zvrf = *info;

  if (! zvrf)
    {
      zvrf = platform_vrf_alloc (vrf_id);
      *info = (void *)zvrf;
      router_id_init (zvrf);
    }

  return 0;
}

/* Callback upon enabling a VRF. */
static int
platform_vrf_enable (vrf_id_t vrf_id, void **info)
{
  struct platform_vrf *zvrf = (struct platform_vrf *) (*info);

  assert (zvrf);

#if defined (HAVE_RTADV)
  rtadv_init (zvrf);
#endif
  kernel_init (zvrf);
  interface_list (zvrf);
  route_read (zvrf);

  return 0;
}

/* Callback upon disabling a VRF. */
static int
platform_vrf_disable (vrf_id_t vrf_id, void **info)
{
  struct platform_vrf *zvrf = (struct platform_vrf *) (*info);
  struct listnode *list_node;
  struct interface *ifp;

  assert (zvrf);

  rib_close_table (zvrf->table[AFI_IP][SAFI_UNICAST]);
  rib_close_table (zvrf->table[AFI_IP6][SAFI_UNICAST]);

  for (ALL_LIST_ELEMENTS_RO (vrf_iflist (vrf_id), list_node, ifp))
    {
      int operative = if_is_operative (ifp);
      UNSET_FLAG (ifp->flags, IFF_UP);
      if (operative)
        if_down (ifp);
    }

#if defined (HAVE_RTADV)
  rtadv_terminate (zvrf);
#endif
  kernel_terminate (zvrf);

  list_delete_all_node (zvrf->rid_all_sorted_list);
  list_delete_all_node (zvrf->rid_lo_sorted_list);

  return 0;
}




static int module_init(void)
{
  platformd.master = thread_master_create ();
  cmd_init ();
  vty_init (platformd.master);
  memory_init ();
  vty_stdio (vty_do_exit);
  vty_serv_sock (NULL, VTY_PORT);
  //uart4_init(9600);
  //create_task_timer();
  return 0;
}


/* Main startup routine. */
int
main (int argc, char **argv)
{
  module_init();

  thread_main (platformd.master);
  return 0;
}
