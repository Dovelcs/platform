#ifndef _CONFIG_SIMP_H
#define _CONFIG_SIMP_H

//#define ZEBRA_SERV_PATH "/home/zy/quagga-simp/usr/local/etc/zserv.api"
#define GDB_DBG 		0
#define HAVE_UCONTEXT_H 1

#define LOGFILE_MASK 0600

#define HAVE_IPV6 1

#define HAVE_IPV6 1

#define PATH_PLATFORM_PID ""

//#define PLATFORM_VTYSH_PATH "/home/zy/quagga-simp/usr/local/etc/zebra.vty"

#define CONFIGFILE_MASK 0600

#define HAVE_GETGROUPLIST 1

#define TIME_WITH_SYS_TIME 1

#define HAVE_SYS_TIME_H 1

#define HAVE_RUSAGE

#define HAVE_LIMITS_H 1

#define HAVE_INTTYPES_H 1

#define HAVE_STDBOOL_H 1

#define HAVE_FEATURES_H 1

#define HAVE_LINUX_VERSION_H 1

#define HAVE_ASM_TYPES_H 1

#define HAVE_NETINET_IN_H 1

#define HAVE_NET_ROUTE_H 1

#define HAVE_NETLINK 

#define HAVE_NETDB_H 1

#define HAVE_NETINET_ICMP6_H 1

typedef enum {
  bool_false = 0,
  bool_true  = 1
} bool;

#define HAVE_SOCKLEN_T 1

#define HAVE_GLIBC_BACKTRACE 

#define HAVE_SNPRINTF 1

#define HAVE_VSNPRINTF 1

#define HAVE_STRNLEN 1

#define HAVE_STRNDUP 1

#define HAVE_STRUCT_IN_PKTINFO 1

#define WORDS_BIGENDIAN 1

#define LINUX_IPV6 1

#define HAVE_INET_NTOP

#define HAVE_INET_PTON

#define HAVE_INET_ATON 1

#define QUAGGA_NO_DEPRECATED_INTERFACES 1

#define HAVE_STDLIB_H 1

#define HAVE_IF_NAMETOINDEX 1

#define HAVE_IF_INDEXTONAME 1

#define MULTIPATH_NUM 1

#define SYSCONFDIR "./"

#endif
