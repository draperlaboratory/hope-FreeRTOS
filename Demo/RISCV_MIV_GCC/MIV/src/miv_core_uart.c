#include "miv_core_uart.h"

int miv_core_uart_init(miv_core_uart_t *pio, int default_baudrate, int clock_rate) {
  return 0;
}

int miv_core_uart_rxready(miv_core_uart_t *pio) {
  return 0;
}

int miv_core_uart_rxchar(miv_core_uart_t *pio) {
  return 0;
}

int miv_core_uart_txchar(miv_core_uart_t *pio, int c) {
  if (c == '\n') {
    pio->transmit_data = '\r';
    pio->transmit_data = '\n';
  } else {
    pio->transmit_data = c;
  }
  return c;
}
