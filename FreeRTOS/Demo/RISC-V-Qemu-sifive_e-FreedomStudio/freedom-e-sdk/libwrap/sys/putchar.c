/* See LICENSE of license details. */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "platform.h"
#include "stub.h"
#include "weak_under_alias.h"

int __wrap_putchar(const char *s)
{
  while (UART0_REG(UART_REG_TXFIFO) & 0x80000000) ;
  UART0_REG(UART_REG_TXFIFO) = *s;

  return 0;
}
weak_under_alias(putchar);
