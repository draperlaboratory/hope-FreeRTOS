#include "bsp.h"
#include "uart_16550.h"
#include "plic_driver.h"


// to communicate with the debugger in spike
volatile uint64_t tohost __attribute__((aligned(64)));
volatile uint64_t fromhost __attribute__((aligned(64)));

plic_instance_t Plic;

void prvSetupHardware( void )
{
    // Resets PLIC, threshold 0, nothing enabled
    PLIC_init(&Plic, PLIC_BASE_ADDR, 4, 4);

    // Set priorities
    PLIC_set_priority(&Plic, PLIC_SOURCE_UART0, PLIC_PRIORITY_UART0);
    PLIC_set_priority(&Plic, PLIC_SOURCE_UART1, PLIC_PRIORITY_UART1);
    PLIC_set_priority(&Plic, PLIC_SOURCE_IIC, PLIC_PRIORITY_IIC);
    PLIC_set_priority(&Plic, PLIC_SOURCE_SPI, PLIC_PRIORITY_SPI);

    // Enable external interrupts
    // TODO: the interrupts should be enabled from the peripherals
    PLIC_enable_interrupt(&Plic,PLIC_SOURCE_UART0);
    PLIC_enable_interrupt(&Plic,PLIC_SOURCE_UART1);
    PLIC_enable_interrupt(&Plic,PLIC_SOURCE_IIC);
    PLIC_enable_interrupt(&Plic,PLIC_SOURCE_SPI);

	UART_init();
	//I2C_init();
	//SPI_init();
	//GPIO_init();
 	//PLIC_init();
 	//UART_init( &g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, ( DATA_8_BITS | NO_PARITY ) );
}
// Define an external interrupt handler
// cause = 0x8000000b == Machine external interrupt
void external_interrupt_handler( uint32_t cause ) {
    if (cause != 0x8000000b) {
        // unknown cause
        __asm volatile( "ebreak" );
    }

    plic_source src = PLIC_claim_interrupt(&Plic);

    // this can be replaced by a vector table effectively
    // int status = HandlerTable[src]();
    switch(src) {
        case PLIC_SOURCE_UART0:
            // this is UART0
            break;
        case PLIC_SOURCE_UART1:
            // call UART1 handler
            break;
        case PLIC_SOURCE_IIC:
            // call iic handler
            break;
        case PLIC_SOURCE_SPI:
            // call SPI handler
            break;
        default:
            // unknown source
            __asm volatile( "ebreak" );
    }

    // TODO: check return status from the handlers
    // TODO: should we process all interrupts while we are here?

    // clear interrupt
    PLIC_complete_interrupt(&Plic,src);
}

