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
