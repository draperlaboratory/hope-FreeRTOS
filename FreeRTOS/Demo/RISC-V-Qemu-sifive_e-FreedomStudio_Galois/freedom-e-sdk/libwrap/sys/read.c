/* See LICENSE of license details. */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "platform.h"
#include "stub.h"
#include "weak_under_alias.h"

enum {
  UART_RXBUF_SIZE = (1 << 3),
  UART_RXBUF_MASK = (UART_RXBUF_SIZE - 1),
};

static volatile uint8_t rxbuf_head;
static volatile uint8_t rxbuf_tail;
static char rxbuf[UART_RXBUF_SIZE];

static int uart_probe_rx()
{
  int32_t c;

  c = UART0_REG(UART_REG_RXFIFO);

  if(c >= 0) {
    rxbuf[rxbuf_head] = (char)c;
    rxbuf_head++;
    rxbuf_head &= UART_RXBUF_MASK;
    return 0;
  }

  return -1;
}

static int uart_get_char(char *ptr, int blocking)
{
  int busy;

  do {
    uart_probe_rx();
    busy = (rxbuf_head == rxbuf_tail);
  } while (blocking && busy);

  *ptr = rxbuf[rxbuf_tail];
  rxbuf_tail++;
  rxbuf_tail &= UART_RXBUF_MASK;
  return 0;
}

static ssize_t uart_read(char *ptr, size_t len, char terminator, int blocking) {
  ssize_t i;
  
  for (i = 0; i < len; i++) {
    if (uart_get_char(&ptr[i], blocking) != 0) {
      break;
    }

    if (ptr[i] == terminator) {
      break;
    }
  }
  return i;
}

ssize_t __wrap_read(int fd, void* ptr, size_t len)
{
  if (isatty(fd)) {
    return uart_read(ptr, len, '\r', 1);
  }

  return _stub(EBADF);
}
weak_under_alias(read);
