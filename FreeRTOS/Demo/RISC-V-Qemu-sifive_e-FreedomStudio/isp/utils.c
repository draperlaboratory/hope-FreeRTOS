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

#include <stdarg.h>
#include <stdio.h>

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

unsigned long sys_GetWallTimestampUs(void)
{
    /* TBD on real FPGA hw */
  return uiPortGetWallTimestampUs();
}

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

/* XXX: Hack to pull in __wrap__ syms to fix a linker error */
void wrap_hack()
{
  sbrk();
  close();
  lseek();
  read();
  fstat();
  malloc(0);
}
