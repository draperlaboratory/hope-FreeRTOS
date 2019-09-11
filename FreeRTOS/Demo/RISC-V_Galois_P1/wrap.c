#include <string.h>
#include <stdint.h>
#include <stdio.h>

int __wrap_printf(const char *s, ...)
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

int __wrap_puts(const char *s)
{
  while (*s != '\0') {
    uart0_txchar(*s);

    if (*s == '\n') {
      uart0_txchar('\r');
    }

    ++s;
  }

  return 0;
}
