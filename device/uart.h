#ifndef UART_H
#define UART_H

int uart4_init(unsigned int baud_rate);
void uart4_close(void);
int uart4_send(unsigned char *buf, int len);
int uart4_recv();
int uart4_get_recv_buf(unsigned char *buf, int len);

#endif // UART_H
