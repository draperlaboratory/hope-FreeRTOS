#ifndef __NS16550_H__
#define __NS16550_H__

#include <stdint.h>
#include "isp32_mpu/memory_map.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NS16550_PEX_BASE  SOC_ADDR_UART0
#define NS16550_AP_BASE  SOC_ADDR_UART1
//#define NS16550_CLOCK_RATE  (100000000ULL) // 100MHz
//#define NS16550_CLOCK_RATE  (3686400ULL)
#define NS16550_CLOCK_RATE  (12500000ULL)

#define NS16550_DEFAULT_BAUD_RATE 115200


enum __attribute__ ((__packed__)) ier_t
{
  IER_ERBI = (1<<0),
  IER_ETBEI = (1<<1),
  IER_ELSI = (1<<2),
  IER_EDSSI = (1<<3),
  IER_SLEEP = (1<<4),  // 16650 only
};

enum __attribute__ ((__packed__)) iir_t
{
  IIR_IP = 1,
  IIR_NONE = 1,
  IIR_RLSI = 6,
  IIR_RDAI = 4,
  IIR_CTOI = 0xc,
  IIR_THRI = 2,
  IIR_MSRI = 0,
  IIR_F64E = 0x20,
  IIR_FE = 0xc0,
};

enum __attribute__ ((__packed__)) fcr_t
{
  FCR_FE = 1,
  FCR_RXFR = 2,
  FCR_TXFR = 4,
  FCR_DMS = 8,
  FCR_F64E = 0x20,
  FCR_RT1 = 0,
  FCR_RT4 = 0x40,
  FCR_RT8 = 0x80,
  FCR_RT14 = 0xc0,
  FCR_RT16 = 0x40,
  FCR_RT32 = 0x80,
  FCR_RT56 = 0xc0,
};

enum __attribute__ ((__packed__)) lcr_t
{
  LCR_WLS5 = 0,
  LCR_WLS6 = 1,
  LCR_WLS7 = 2,
  LCR_WLS8 = 3,
  LCR_STB = 4,
  LCR_PEN = 8,
  LCR_EPS = 0x10,
  LCR_SP = 0x20,
  LCR_BC = 0x40,
  LCR_DLAB = 0x80,
};

enum __attribute__ ((__packed__)) mcr_t
{
  MCR_DTR = 1,
  MCR_RTS = 2,
  MCR_OUT1 = 4,
  MCR_OUT2 = 8,
  MCR_LOOP = 0x10,
  MCR_AFE = 0x20,
};

enum __attribute__ ((__packed__)) lsr_t
{
  LSR_DR = 1,
  LSR_OE = 2,
  LSR_PE = 4,
  LSR_FE = 8,
  LSR_BI = 0x10,
  LSR_THRE = 0x20,
  LSR_TEMT = 0x40,
  LSR_RXFE = 0x80,
};

enum __attribute__ ((__packed__)) msr_t
{
  MSR_DCTS = 1,
  MSR_DDSR = 2,
  MSR_TERI = 4,
  MSR_DDCD = 8,
  MSR_CTS = 0x10,
  MSR_DSR = 0x20,
  MSR_RI = 0x40,
  MSR_DCD = 0x80,
};

//#define ALIGN_UART
#ifdef ALIGN_UART
#define UART_ALIGNMENT __attribute__ ((aligned (4)))
#else
#define UART_ALIGNMENT
#endif

typedef struct UART_ALIGNMENT ns16550_pio
{
  // 0x000
  union UART_ALIGNMENT {
    const volatile uint8_t rbr;
    volatile uint8_t thr;
    volatile uint8_t dll;
  };

  // 0x0004
  union UART_ALIGNMENT {
    volatile uint8_t dlm;
    volatile enum ier_t ier;
  };

  // 0x008
  union UART_ALIGNMENT {
    const volatile enum iir_t iir;
    volatile enum fcr_t fcr;
  };

  // 0x00c
  volatile enum lcr_t lcr UART_ALIGNMENT;

  // 0x010
  volatile enum mcr_t mcr UART_ALIGNMENT;

  // 0x014
  volatile enum lsr_t lsr UART_ALIGNMENT;

  // 0x018
  volatile uint8_t msr UART_ALIGNMENT;

  // 0x01c
  volatile uint8_t scr UART_ALIGNMENT;
} ns16550_pio_t;

int ns16550_init(ns16550_pio_t *pio, int default_baudrate, int clock_rate);
int ns16550_rxready(ns16550_pio_t *pio);
int ns16550_rxchar(ns16550_pio_t *pio);
int ns16550_txchar(ns16550_pio_t *pio, int c);
void ns16550_flush(ns16550_pio_t *pio);
enum lsr_t ns16550_get_lsr(ns16550_pio_t *pio);

#ifdef __cplusplus
}
#endif

#endif
