/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "portexception.h"

/* Standard includes. */
#include "string.h"

#ifdef configCLINT_BASE_ADDRESS
	#warning The configCLINT_BASE_ADDRESS constant has been deprecated.  configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS are currently being derived from the (possibly 0) configCLINT_BASE_ADDRESS setting.  Please update to define configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS dirctly in place of configCLINT_BASE_ADDRESS.
#endif

#ifndef configMTIME_BASE_ADDRESS
	#warning configMTIME_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtime register then set configMTIME_BASE_ADDRESS to the mapped address.  Otherwise set configMTIME_BASE_ADDRESS to 0.
#endif

#ifndef configMTIMECMP_BASE_ADDRESS
	#warning configMTIMECMP_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtimecmp register then set configMTIMECMP_BASE_ADDRESS to the mapped address.  Otherwise set configMTIMECMP_BASE_ADDRESS to 0.
#endif

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
	#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
	#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
to use a statically allocated array as the interrupt stack.  Alternative leave
configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
linker variable names __freertos_irq_stack_top has the same value as the top
of the stack used by main.  Using the linker script method will repurpose the
stack that was used by main before the scheduler was started for use as the
interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
	static __attribute__ ((aligned(16))) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 };
  /* SSITH HOPE.  I don't think the configISR_STACK_SIZE_WORDS option had been
     tested, because the previous version of this line resulted in an unaligned
     stack that trips an assert later.  I now appropriately align the stack,
     although I think it's overly conservative because the mask is wrt bytes and
     the array is words.   A fix like this should probably be upstreamed.  */
	const StackType_t xISRStackTop = ( StackType_t ) &( xISRStack[ ( ( configISR_STACK_SIZE_WORDS - 1 ) & ~portBYTE_ALIGNMENT_MASK ) ] );
    
    /* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
	the task stacks, and so will legitimately appear in many positions within
	the ISR stack. */
	#define portISR_STACK_FILL_BYTE	0xee
#else
	extern const uint32_t __freertos_irq_stack_top[];
	const StackType_t xISRStackTop = ( StackType_t ) __freertos_irq_stack_top;
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void ) __attribute__(( weak ));

/*-----------------------------------------------------------*/

/* Used to program the machine timer compare register. */
uint64_t ullNextTime = 0ULL;
const uint64_t *pullNextTime = &ullNextTime;
const size_t uxTimerIncrementsForOneTick = ( size_t ) ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ); /* Assumes increment won't go over 32-bits. */
uint32_t const ullMachineTimerCompareRegisterBase = configMTIMECMP_BASE_ADDRESS;
volatile uint64_t * pullMachineTimerCompareRegister = NULL;

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 )
	#warning This path not tested, or even compiled yet.

	static const uint8_t ucExpectedStackBytes[] = {
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE };	\

	#define portCHECK_ISR_STACK() configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else
	/* Define the function away. */
	#define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

/*-----------------------------------------------------------*/

#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )

	void vPortSetupTimerInterrupt( void )
	{
	uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
	volatile uint32_t * const pulTimeHigh = ( volatile uint32_t * const ) ( ( configMTIME_BASE_ADDRESS ) + 4UL ); /* 8-byte typer so high 32-bit word is 4 bytes up. */
	volatile uint32_t * const pulTimeLow = ( volatile uint32_t * const ) ( configMTIME_BASE_ADDRESS );
	volatile uint32_t ulHartId;

		__asm volatile( "csrr %0, mhartid" : "=r"( ulHartId ) );
		pullMachineTimerCompareRegister  = ( volatile uint64_t * ) ( ullMachineTimerCompareRegisterBase + ( ulHartId * sizeof( uint64_t ) ) );

		do
		{
			ulCurrentTimeHigh = *pulTimeHigh;
			ulCurrentTimeLow = *pulTimeLow;
		} while( ulCurrentTimeHigh != *pulTimeHigh );

		ullNextTime = ( uint64_t ) ulCurrentTimeHigh;
		ullNextTime <<= 32ULL; /* High 4-byte word is 32-bits up. */
		ullNextTime |= ( uint64_t ) ulCurrentTimeLow;
		ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
		*pullMachineTimerCompareRegister = ullNextTime;

		/* Prepare the time to use after the next tick interrupt. */
		ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
	}

#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIME_BASE_ADDRESS != 0 ) */
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
extern void xPortStartFirstTask( void );

	#if( configASSERT_DEFINED == 1 )
	{
		volatile uint32_t mtvec = 0;

		/* Check the least significant two bits of mtvec are 00 - indicating
		single vector mode. */
		__asm volatile( "csrr %0, mtvec" : "=r"( mtvec ) );
		configASSERT( ( mtvec & 0x03UL ) == 0 );

		/* Check alignment of the interrupt stack - which is the same as the
		stack that was being used by main() prior to the scheduler being
		started. */
		configASSERT( ( xISRStackTop & portBYTE_ALIGNMENT_MASK ) == 0 );

		#ifdef configISR_STACK_SIZE_WORDS
		{
			memset( ( void * ) xISRStack, portISR_STACK_FILL_BYTE, sizeof( xISRStack ) );
		}
		#endif	 /* configISR_STACK_SIZE_WORDS */
	}
	#endif /* configASSERT_DEFINED */

	/* If there is a CLINT then it is ok to use the default implementation
	in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
	configure whichever clock is to be used to generate the tick interrupt. */
	vPortSetupTimerInterrupt();

	#if( ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) )
	{
		/* Enable mtime and external interrupts.  1<<7 for timer interrupt, 1<<11
		for external interrupt.  _RB_ What happens here when mtime is not present as
		with pulpino? */
		__asm volatile( "csrs mie, %0" :: "r"(0x880) );
	}
	#else
	{
		/* Enable external interrupts. */
		__asm volatile( "csrs mie, %0" :: "r"(0x800) );
	}
	#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) */

	xPortStartFirstTask();

	/* Should not get here as after calling xPortStartFirstTask() only tasks
	should be executing. */
	return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented. */
	for( ;; );
}

void vPortHandleInterrupt( void )
{
   	/* Not implemented. */
}

/* exception debugging */
void is_exception_print( uint32_t cause, uint32_t mepc, uint32_t status ) {
    const char *privilege_levels[] = {"User", "Supervisor", "Reserved", "Machine"};
    uint32_t *sp;
    int i;

    printf("\nEXCEPTION DETECTED\n  Type: ");
    switch( global_exception_mc.mcause ) {

      case portRV_INSTRUCTION_ADDRESS_MISALIGNED:
          printf("instruction addr misaligned\n");
          break;
      case portRV_INSTRUCTION_ACCESS_FAULT:
          printf("instruction access fault\n");
          break;
      case portRV_ILLEGAL_INSTRUCTION:
          printf("illegal instruction\n");
          break;
      case portRV_LOAD_ADDRESS_MISALIGNED:
          printf("load address misaligned\n");
          break;
      case portRV_LOAD_ACCESS_FAULT:
          printf("load access fault\n");
          break;
      case portRV_STORE_ADDRESS_MISALIGNED:
          printf("store/AMO addr misaligned\n");
          break;
      case portRV_STORE_ACCESS_FAULT:
          printf("store/AMO access fualt\n");
          break;
      case portRV_INSTRUCTION_PAGE_FAULT:
          printf("instruction page fault\n");
          break;
      case portRV_LOAD_PAGE_FAULT:
          printf("load page fault\n");
          break;
      case portRV_STORE_PAGE_FAULT:
          printf("store/AMO page fault\n");
          break;
      default:
          printf("Unknown MCAUSE: 0x%x\n", global_exception_mc.mcause);
      }

    printf("  MISA: 0x%x\n", global_exception_mc.misa);
    printf("  MEPC: 0x%x\n", global_exception_mc.mepc);
    printf("  MTVAL: 0x%x\n", global_exception_mc.mtval);

    printf("  MSTATUS: 0x%x\n", global_exception_mc.mstatus);
    printf("    Previous privilege mode: %s\n",
        privilege_levels[((0x3 << 11) & global_exception_mc.mstatus) >> 11]);

#if (configEXCEPTION_HANDLER_STACK_DUMP != 0)
    printf("  Stack dump:\n");
    printf("    (stack pointer = 0x%x)\n", global_exception_mc.x2);
    sp = (uint32_t*)global_exception_mc.x2;
    for ( i = 0; i < 32; i++, sp++ ) {
        printf(    "    @ 0x%x: 0x%x", sp, *sp);
        if ( !i )
            printf(" (sp)");
        printf("\n");
    }
#endif 

#if (configEXCEPTION_HANDLER_REGISTER_DUMP != 0)
    printf("  Register file:\n");
    printf("    ra (x1): 0x%x\n", global_exception_mc.x1);
    printf("    sp (x2): 0x%x\n", global_exception_mc.x2);
    printf("    gp (x3): 0x%x\n", global_exception_mc.x3);
    printf("    tp (x4): 0x%x\n", global_exception_mc.x4);
    printf("    t0 (x5): 0x%x\n", global_exception_mc.x5);
    printf("    t1 (x6): 0x%x\n", global_exception_mc.x6);
    printf("    t2 (x7): 0x%x\n", global_exception_mc.x7);
    printf("    t3 (x28): 0x%x\n", global_exception_mc.x28);
    printf("    t4 (x29): 0x%x\n", global_exception_mc.x29);
    printf("    t5 (x30): 0x%x\n", global_exception_mc.x30);
    printf("    t6 (x31): 0x%x\n", global_exception_mc.x31);
    printf("    s0/fp (x8): 0x%x\n", global_exception_mc.x8);
    printf("    s1 (x9): 0x%x\n", global_exception_mc.x9);
    printf("    s2 (x18): 0x%x\n", global_exception_mc.x18);
    printf("    s3 (x19): 0x%x\n", global_exception_mc.x19);
    printf("    s4 (x20): 0x%x\n", global_exception_mc.x20);
    printf("    s5 (x21): 0x%x\n", global_exception_mc.x21);
    printf("    s6 (x22): 0x%x\n", global_exception_mc.x22);
    printf("    s7 (x23): 0x%x\n", global_exception_mc.x23);
    printf("    s8 (x24): 0x%x\n", global_exception_mc.x24);
    printf("    s9 (x25): 0x%x\n", global_exception_mc.x25);
    printf("    s10 (x26): 0x%x\n", global_exception_mc.x26);
    printf("    s11 (x27): 0x%x\n", global_exception_mc.x27);
    printf("    a0 (x10): 0x%x\n", global_exception_mc.x10);
    printf("    a1 (x11): 0x%x\n", global_exception_mc.x11);
    printf("    a2 (x12): 0x%x\n", global_exception_mc.x12);
    printf("    a3 (x13): 0x%x\n", global_exception_mc.x13);
    printf("    a4 (x14): 0x%x\n", global_exception_mc.x14);
    printf("    a5 (x15): 0x%x\n", global_exception_mc.x15);
    printf("    a6 (x16): 0x%x\n", global_exception_mc.x16);
    printf("    a7 (x17): 0x%x\n", global_exception_mc.x17);	 
#endif

    return;
}
