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

    // initialize peripherals
	UART_init();
}
// Define an external interrupt handler
// cause = 0x8000000b == Machine external interrupt
void external_interrupt_handler( uint32_t cause ) {
    if (cause != 0x8000000b) {
        // unknown cause
        // TODO: handle better
        __asm volatile( "ebreak" );
    }

    plic_source source_id = PLIC_claim_interrupt(&Plic);

    if ((source_id >=1 ) && (source_id < PLIC_NUM_INTERRUPTS)) {
        Plic.HandlerTable[source_id].Handler(Plic.HandlerTable[source_id].CallBackRef);
    }

    // clear interrupt
    PLIC_complete_interrupt(&Plic,source_id);
}

