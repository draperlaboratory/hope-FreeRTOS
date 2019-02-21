#ifndef RISCV_BLUESPEC_BSP_H
#define RISCV_BLUESPEC_BSP_H

#include "stdint.h"
#include "plic_driver.h"

#define PLIC_BASE_ADDR (0xC000000ULL)

#define PLIC_SOURCE_UART0 0x1
#define PLIC_SOURCE_UART1 0x2
#define PLIC_SOURCE_IIC 0x3
#define PLIC_SOURCE_SPI 0x4

#define PLIC_PRIORITY_UART0 0x1
#define PLIC_PRIORITY_UART1 0x1
#define PLIC_PRIORITY_IIC 0x3
#define PLIC_PRIORITY_SPI 0x2

extern plic_instance_t Plic;

/* Prepare haredware to run the demo. */
void prvSetupHardware( void );

void external_interrupt_handler( uint32_t cause );

#endif /* RISCV_BLUESPEC_BSP_H */
