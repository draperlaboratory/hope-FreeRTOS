#ifndef __UART_16550_H__
#define __UART_16550_H__

#include "bsp.h"
#include <stdbool.h>

#if BSP_USE_UART0
bool uart0_rxready(void);
char uart0_rxchar(void);
char uart0_txchar(char c);
int uart0_rxbuffer(char *ptr, int len);
int uart0_txbuffer(char *ptr, int len);
void uart0_init(void);
#endif

#if BSP_USE_UART1
bool uart1_rxready(void);
char uart1_rxchar(void);
char uart1_txchar(char c);

/**
 *  Returns 0 if error happened, otherwise return the number of read characters
 */
//@ requires \valid (ptr + len);
//@ ensures \result == 0 <=> // TODO: we are in the error state
//@ ensures \result <= len;
//@ modifies (ptr + (0..\result));
uint8_t uart1_rxbuffer(char *buffer, uint8_t len);
int uart1_txbuffer(char *ptr, int len);
void uart1_init(void);
#endif

#endif
