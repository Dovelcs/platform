#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "uart.h"
#include <signal.h>
#include <time.h>

int glUart4Fd = -1;

//打开/dev/ttymxc4串口
//指定波特率
//使用全局变量glUart4Fd保存串口文件描述符
int uart4_init(unsigned int baud_rate) {
    struct termios options;
    speed_t speed;
    int fd;

    // Open UART4,使用阻塞模式

    fd = open("/dev/ttymxc4", O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // Set the baud rate
    switch (baud_rate) {
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400:
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        default:
            fprintf(stderr, "Unsupported baud rate\n");
            return -1;
    }
    if (cfsetispeed(&options, speed) < 0 || cfsetospeed(&options, speed) < 0) {
        perror("cfsetispeed/cfsetospeed");
        return -1;
    }

    // Set the remaining options
    if (tcgetattr(fd, &options) < 0) {
        perror("tcgetattr");
        return -1;
    }
    options.c_cflag |= CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 1;
    if (tcsetattr(fd, TCSANOW, &options) < 0) {
        perror("tcsetattr");
        return -1;
    }

    // Save the file descriptor to a global variable
    glUart4Fd = fd;

    return 0;
}


//关闭串口
void uart4_close(void) {
    close(glUart4Fd);
}


//发送数据
int uart4_send(unsigned char *buf, int len) {
    int ret;

    ret = write(glUart4Fd, buf, len);
    if (ret == -1) {
        perror("write");
        return -1;
    }

    return ret;
}

unsigned char glaRecvBuf[1024];
//接收数据,到缓存区中
int uart4_recv() {
    int ret;
    static int i = 0;
    ret = read(glUart4Fd, glaRecvBuf, 1024);
    if (ret == -1) {
        perror("read");
        return -1;
    }
    printf("uart4_recv: %s\r\n", glaRecvBuf);
    return ret;
}

int uart4_get_recv_buf(unsigned char *buf, int len) {
    memcpy(buf, glaRecvBuf, len);
    memset(glaRecvBuf, 0, 1024);
    return 0;
}
