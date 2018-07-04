
#include <stdint.h>
#include "ns16550.h"


//#define DEFAULT_BAUDRATE  (9600)
//#define DEFAULT_BAUDRATE  (115200)
//#define DEFAULT_BAUDRATE  (4800)

int ns16550_init(ns16550_pio_t *pio, int default_baudrate, int clock_rate)
{
  uint32_t divisor;

  pio->ier = 0;

  divisor = clock_rate / (16 * default_baudrate);
//  divisor = 115200 / (DEFAULT_BAUDRATE);
  pio->lcr |= LCR_DLAB;
  pio->dll = divisor & 0xff;
  pio->dlm = (divisor >> 8) & 0xff;
  pio->lcr &= ~LCR_DLAB;

  pio->lcr = LCR_WLS8;
  pio->fcr = FCR_FE;
  pio->mcr = MCR_RTS;

  return 0;
}

int ns16550_rxready(ns16550_pio_t *pio)
{
  return (pio->lsr & LSR_DR) != 0;
}


int ns16550_rxchar(ns16550_pio_t *pio)
{
  while ((pio->lsr & LSR_DR) == 0)
    ;  // nothing

  return pio->rbr;
}


int ns16550_txchar(ns16550_pio_t *pio, int c)
{
  while ((pio->lsr & LSR_THRE) == 0)
    ;  // nothing

  pio->thr = c;

  return c;
}


void ns16550_flush(ns16550_pio_t *pio)
{
  while ((pio->lsr & LSR_TEMT) == 0)
    ;  // nothing
}

enum lsr_t ns16550_get_lsr(ns16550_pio_t *pio)
{
  return pio->lsr;
}
