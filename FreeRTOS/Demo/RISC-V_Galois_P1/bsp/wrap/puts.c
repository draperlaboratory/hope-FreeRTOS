#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include "uart.h"
#include "weak_under_alias.h"

int __wrap_puts(const char *s)
{
  int len = 0;
  while (*s != '\0') {
    uart0_txchar(*s);

    if (*s == '\n') {
      uart0_txchar('\r');
    }

    ++len;
    ++s;
  }

  return len;
}
weak_under_alias(puts);
