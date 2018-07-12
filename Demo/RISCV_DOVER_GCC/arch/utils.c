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

#include "utils.h"
#include "ns16550.h"
#include <stdarg.h>
#include <stdio.h>

//static miv_core_uart_t * pio = (miv_core_uart_t *)(void*)MIV_COREUART_BASE;
static ns16550_pio_t *pio = (ns16550_pio_t *)NS16550_AP_BASE;

void vprintf_uart(const char* s, va_list vl)
{
  char buf[256];

  const char *p = &buf[0];
  
  vsnprintf(buf, sizeof buf, s, vl);

  while (*p) {
    ns16550_txchar(pio, *p++);
  }
}

void printf_uart(const char* s, ...)
{
  va_list vl;

  va_start(vl, s);
  vprintf_uart(s, vl);
  va_end(vl);
}


void panic(const char* msg, ...) {
  va_list vl;
  va_start(vl, msg);
  vprintf_uart(msg, vl);
  va_end(vl);
  while (1) ;
}

