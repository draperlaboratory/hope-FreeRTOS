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
#include "miv_core_uart.h"
#include <stdarg.h>
#include <stdio.h>

static miv_core_uart_t * pio = (miv_core_uart_t *)(void*)MIV_COREUART_BASE;

void printf_uart(const char* s, ...)
{
  char buf[128];
  va_list vl;

  const char *p = &buf[0];
  
  va_start(vl, s);
  vsnprintf(buf, sizeof buf, s, vl);
  va_end(vl);

  while (*p) {
    miv_core_uart_txchar(pio, *p++);
  }
}


void panic(const char* msg, ...) {
  va_list vl;
  va_start(vl, msg);
  printf_uart(msg, vl);
  va_end(vl);
  while (1) ;
}

