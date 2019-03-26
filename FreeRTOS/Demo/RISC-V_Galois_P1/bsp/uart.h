
#ifndef __UART_16550_H__
#define __UART_16550_H__

int uart0_rxready(void);
char uart0_rxchar(void);
char uart0_txchar(char c);
int uart0_rxbuffer(char *ptr, int len);
int uart0_txbuffer(char *ptr, int len);
void uart0_init(void);

#endif
