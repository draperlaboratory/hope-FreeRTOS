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
#include "platform.h"
#include <stdarg.h>
#include <stdio.h>

#if 0
void vprintf_uart(const char* s, va_list vl)
{
  char buf[256];

  const char *p = &buf[0];
  
  vsnprintf(buf, sizeof buf, s, vl);

  while (*p) {
    putchar(*p++);
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

void printk(const char*, ...);
void printk(const char* s, ...)
{
  va_list vl;

  va_start(vl, s);
  printf_uart(s, vl);
  va_end(vl);
}

int t_printf(const char *s, ...)
{
  va_list vl;

  va_start(vl, s);
  vprintf_uart(s, vl);
  va_end(vl);

  return 0;
}

/*
int t_printf(const char *s, ...)
{
  char buf[256];
  va_list vl;

  const char *p = &buf[0];

  va_start(vl, s);
  vsnprintf(buf, sizeof buf, s, vl);
  va_end(vl);

  puts(p);

  return 0;
}
*/
#endif

uint32_t get_usec_time()
{
  return (uint32_t)get_timer_value();
}

uint32_t get_inst_ret()
{
  uint64_t instret;
  asm volatile ("csrr %0, 0xc02 " : "=r"(instret));
  return instret;
}

uint32_t uiPortGetWallTimestampUs()
{
  return (uint32_t)get_timer_value();
}
