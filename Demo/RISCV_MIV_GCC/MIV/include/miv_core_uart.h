/*
 * Copyright © 2017-2018 Dover Microsystems, Inc.
 * All rights reserved. 
 *
 * Use and disclosure subject to the following license. 
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef MIV_CORE_UART_H
#define MIV_CORE_UART_H

#include <stdint.h>

#define UART_ALIGNMENT __attribute__ ((aligned (4)))

#define MIV_COREUART_BASE 0x70001000

typedef struct UART_ALIGNMENT miv_core_uart {
  uint8_t UART_ALIGNMENT transmit_data;
  uint8_t UART_ALIGNMENT receive_data;
  uint8_t UART_ALIGNMENT control1;
  uint8_t UART_ALIGNMENT control2;
  uint8_t UART_ALIGNMENT status;
  uint8_t UART_ALIGNMENT control3;
} miv_core_uart_t;

int miv_core_uart_init(miv_core_uart_t *pio, int default_baudrate, int clock_rate);
int miv_core_uart_rxready(miv_core_uart_t *pio);
int miv_core_uart_rxchar(miv_core_uart_t *pio);
int miv_core_uart_txchar(miv_core_uart_t *pio, int c);

#endif
