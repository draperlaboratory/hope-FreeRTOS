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

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code which tailors the port to a specific RISC-V chip:
 *
 * + The code that is common to all RISC-V chips is implemented in
 *   FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S.  There is only one
 *   portASM.S file because the same file is used no matter which RISC-V chip is
 *   in use.
 *
 * + The code that tailors the kernel's RISC-V port to a specific RISC-V
 *   chip is implemented in freertos_risc_v_chip_specific_extensions.h.  There
 *   is one freertos_risc_v_chip_specific_extensions.h that can be used with any
 *   RISC-V chip that both includes a standard CLINT and does not add to the
 *   base set of RISC-V registers.  There are additional
 *   freertos_risc_v_chip_specific_extensions.h files for RISC-V implementations
 *   that do not include a standard CLINT or do add to the base set of RISC-V
 *   registers.
 *
 * CARE MUST BE TAKEN TO INCLDUE THE CORRECT
 * freertos_risc_v_chip_specific_extensions.h HEADER FILE FOR THE CHIP
 * IN USE.  To include the correct freertos_risc_v_chip_specific_extensions.h
 * header file ensure the path to the correct header file is in the assembler's
 * include path.
 *
 * This freertos_risc_v_chip_specific_extensions.h is for use on RISC-V chips
 * that include a standard CLINT and do not add to the base set of RISC-V
 * registers.
 *
 */
#if __riscv_xlen == 64
	#define portWORD_SIZE 8
	#define STORE    sd
	#define LOAD     ld
#elif __riscv_xlen == 32
	#define portWORD_SIZE 4
	#define STORE    sw
	#define LOAD     lw
#else
	#error Assembler did not define __riscv_xlen
#endif

#include "freertos_risc_v_chip_specific_extensions.h"
#include "portexception.h"

/* Check the freertos_risc_v_chip_specific_extensions.h and/or command line
definitions. */
#ifndef portasmHAS_CLINT
	#error freertos_risc_v_chip_specific_extensions.h must define portasmHAS_CLINT to either 1 (CLINT present) or 0 (clint not present).
#endif

#ifndef portasmHANDLE_INTERRUPT
	#error portasmHANDLE_INTERRUPT must be defined to the function to be called to handle external/peripheral interrupts.  portasmHANDLE_INTERRUPT can be defined on the assmbler command line or in the appropriate freertos_risc_v_chip_specific_extensions.h header file.
#endif

#ifndef portasmMSI_HANDLER
        #define portasmMSI_HANDLER as_yet_unhandled
#endif

/* Only the standard core registers are stored by default.  Any additional
registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
specific version of freertos_risc_v_chip_specific_extensions.h.  See the notes
at the top of this file. */
#define portCONTEXT_SIZE ( 30 * portWORD_SIZE )

/* NOTE: these values correspond to offsets in the mc_t struct */
#define MC_MSTATUS_OFF 32
#define MC_MTVEC_OFF 33
#define MC_MIP_OFF 34
#define MC_MIE_OFF 35
#define MC_MEPC_OFF 36
#define MC_MCAUSE_OFF 37
#define MC_MTVAL_OFF 38
#define MC_MISA_OFF 39

#define SIFIVE_TEST_ADDR 0x00100000
#define FINISHER_FAIL 0x3333

.global xPortStartFirstTask
.global freertos_risc_v_trap_handler
.global pxPortInitialiseStack
.extern pxCurrentTCB
.extern ulPortTrapHandler
.extern vTaskSwitchContext
.extern Timer_IRQHandler
.extern pullMachineTimerCompareRegister
.extern pullNextTime
.extern uxTimerIncrementsForOneTick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
.extern xISRStackTop

.extern global_exception_mc

/*-----------------------------------------------------------*/
test_dev_addr: .word SIFIVE_TEST_ADDR
finish_val: .word FINISHER_FAIL
/*-----------------------------------------------------------*/

/* save cpu context to structure */	
.macro STRUCT_SAVE_CONTEXT

    // store a0 and x5 for use
    addi sp, sp, -2 * portWORD_SIZE
    STORE a0, 0(sp)
    STORE x5, portWORD_SIZE(sp)

    la a0, global_exception_mc

    STORE x1, 1 * portWORD_SIZE( a0 )

    // x2 is sp, get regular sp back to save
    addi sp, sp, 2 * portWORD_SIZE
    STORE x2, 2 * portWORD_SIZE( a0 )
    addi sp, sp, -2 * portWORD_SIZE

    STORE x3, 3 * portWORD_SIZE( a0 )
    STORE x4, 4 * portWORD_SIZE( a0 )
    STORE x5, 5 * portWORD_SIZE( a0 )
    STORE x6, 6 * portWORD_SIZE( a0 )
    STORE x7, 7 * portWORD_SIZE( a0 )
    STORE x8, 8 * portWORD_SIZE( a0 )
    STORE x9, 9 * portWORD_SIZE( a0 )

    // instead of sw x10, 10 * portWORD_SIZE( a0 )
    LOAD x5, 0(sp)
    STORE x5, 10 * portWORD_SIZE( a0 )

    STORE x11, 11 * portWORD_SIZE( a0 )
    STORE x12, 12 * portWORD_SIZE( a0 )
    STORE x13, 13 * portWORD_SIZE( a0 )
    STORE x14, 14 * portWORD_SIZE( a0 )
    STORE x15, 15 * portWORD_SIZE( a0 )
    STORE x16, 16 * portWORD_SIZE( a0 )
    STORE x17, 17 * portWORD_SIZE( a0 )
    STORE x18, 18 * portWORD_SIZE( a0 )
    STORE x19, 19 * portWORD_SIZE( a0 )
    STORE x20, 20 * portWORD_SIZE( a0 )
    STORE x21, 21 * portWORD_SIZE( a0 )
    STORE x22, 22 * portWORD_SIZE( a0 )
    STORE x23, 23 * portWORD_SIZE( a0 )
    STORE x24, 24 * portWORD_SIZE( a0 )
    STORE x25, 25 * portWORD_SIZE( a0 )
    STORE x26, 26 * portWORD_SIZE( a0 )
    STORE x27, 27 * portWORD_SIZE( a0 )
    STORE x28, 28 * portWORD_SIZE( a0 )
    STORE x29, 29 * portWORD_SIZE( a0 )
    STORE x30, 30 * portWORD_SIZE( a0 )
    STORE x31, 31 * portWORD_SIZE( a0 )

    // save CSRs
    csrr x5, mstatus
    STORE x5, MC_MSTATUS_OFF * portWORD_SIZE( a0 )

    csrr x5, mtvec
    STORE x5, MC_MTVEC_OFF * portWORD_SIZE( a0 )

    csrr x5, mip
    STORE x5, MC_MIP_OFF * portWORD_SIZE( a0 )

    csrr x5, mie
    STORE x5, MC_MIE_OFF * portWORD_SIZE( a0 )

    csrr x5, mepc
    STORE x5, MC_MEPC_OFF * portWORD_SIZE( a0 )

    csrr x5, mcause
    STORE x5, MC_MCAUSE_OFF * portWORD_SIZE( a0 )

    csrr x5, mtval
    STORE x5, MC_MTVAL_OFF * portWORD_SIZE( a0 )

    csrr x5, misa
    STORE x5, MC_MISA_OFF * portWORD_SIZE( a0 )

    /* restore clobbered a0 and x5 */
    LOAD a0, 0(sp)
    LOAD x5, portWORD_SIZE(sp)
    addi sp, sp, 2 * portWORD_SIZE

.endm

.text
.align 8
.global freertos_risc_v_trap_handler
freertos_risc_v_trap_handler:

    /* check for exception */
    /* save scratch regs */
    addi sp, sp, -2 * portWORD_SIZE
    STORE t0, 0(sp)
    STORE t1, portWORD_SIZE(sp)

    csrr t1, mcause

    li t0, portRV_INSTRUCTION_ADDRESS_MISALIGNED
    beq t0, t1, is_exception

    li t0, portRV_INSTRUCTION_ACCESS_FAULT
    beq t0, t1, is_exception

    li t0, portRV_ILLEGAL_INSTRUCTION
    beq t0, t1, is_exception

    li t0, portRV_LOAD_ADDRESS_MISALIGNED
    beq t0, t1, is_exception

    li t0, portRV_LOAD_ACCESS_FAULT
    beq t0, t1, is_exception

    li t0, portRV_STORE_ADDRESS_MISALIGNED
    beq t0, t1, is_exception

    li t0, portRV_STORE_ACCESS_FAULT
    beq t0, t1, is_exception

    li t0, portRV_INSTRUCTION_PAGE_FAULT
    beq t0, t1, is_exception

    li t0, portRV_LOAD_PAGE_FAULT
    beq t0, t1, is_exception

    li t0, portRV_STORE_PAGE_FAULT
    beq t0, t1, is_exception

no_exception: 
    /* restore clobbered a0 and x5 */
    LOAD t0, 0(sp)
    LOAD t1, portWORD_SIZE(sp)
    addi sp, sp, 2 * portWORD_SIZE

    /* beginning of regular trap handler. */
	addi sp, sp, -portCONTEXT_SIZE
	STORE x1, 1 * portWORD_SIZE( sp )
	STORE x5, 2 * portWORD_SIZE( sp )
	STORE x6, 3 * portWORD_SIZE( sp )
	STORE x7, 4 * portWORD_SIZE( sp )
	STORE x8, 5 * portWORD_SIZE( sp )
	STORE x9, 6 * portWORD_SIZE( sp )
	STORE x10, 7 * portWORD_SIZE( sp )
	STORE x11, 8 * portWORD_SIZE( sp )
	STORE x12, 9 * portWORD_SIZE( sp )
	STORE x13, 10 * portWORD_SIZE( sp )
	STORE x14, 11 * portWORD_SIZE( sp )
	STORE x15, 12 * portWORD_SIZE( sp )
	STORE x16, 13 * portWORD_SIZE( sp )
	STORE x17, 14 * portWORD_SIZE( sp )
	STORE x18, 15 * portWORD_SIZE( sp )
	STORE x19, 16 * portWORD_SIZE( sp )
	STORE x20, 17 * portWORD_SIZE( sp )
	STORE x21, 18 * portWORD_SIZE( sp )
	STORE x22, 19 * portWORD_SIZE( sp )
	STORE x23, 20 * portWORD_SIZE( sp )
	STORE x24, 21 * portWORD_SIZE( sp )
	STORE x25, 22 * portWORD_SIZE( sp )
	STORE x26, 23 * portWORD_SIZE( sp )
	STORE x27, 24 * portWORD_SIZE( sp )
	STORE x28, 25 * portWORD_SIZE( sp )
	STORE x29, 26 * portWORD_SIZE( sp )
	STORE x30, 27 * portWORD_SIZE( sp )
	STORE x31, 28 * portWORD_SIZE( sp )

	csrr t0, mstatus					/* Required for MPIE bit. */
	STORE t0, 29 * portWORD_SIZE( sp )

	portasmSAVE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

	LOAD  t0, pxCurrentTCB			/* Load pxCurrentTCB. */
	STORE  sp, 0( t0 )				/* Write sp to first TCB member. */

	csrr a0, mcause
	csrr a1, mepc

test_if_asynchronous:
	srli a2, a0, __riscv_xlen - 1		/* MSB of mcause is 1 if handing an asynchronous interrupt - shift to LSB to clear other bits. */
	beq a2, x0, handle_synchronous		/* Branch past interrupt handing if not asynchronous. */
	STORE a1, 0( sp )					/* Asynch so save unmodified exception return address. */

handle_asynchronous:

#if( portasmHAS_CLINT != 0 )

	test_if_mtimer:						/* If there is a CLINT then the mtimer is used to generate the tick interrupt. */

		addi t0, x0, 1

		slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */
		addi t1, t0, 7					/* 0x8000[]0007 == machine timer interrupt. */
		bne a0, t1, test_if_external_interrupt

		LOAD t0, pullMachineTimerCompareRegister  /* Load address of compare register into t0. */
		LOAD t1, pullNextTime  		/* Load the address of ullNextTime into t1. */

		#if( __riscv_xlen == 32 )

			/* Update the 64-bit mtimer compare match value in two 32-bit writes. */
			lw t2, 0(t1)				/* Load the low word of ullNextTime into t2. */
			lw t3, 4(t1)				/* Load the high word of ullNextTime into t3. */
			sw t2, 0(t0)				/* Store low word of ullNextTime into compare register. */
			sw t3, 4(t0)				/* Store high word of ullNextTime into compare register. */
			lw t0, uxTimerIncrementsForOneTick	/* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
			add t4, t0, t2				/* Add the low word of ullNextTime to the timer increments for one tick (assumes timer increment for one tick fits in 32-bits). */
			sltu t5, t4, t2				/* See if the sum of low words overflowed (what about the zero case?). */
			add t6, t3, t5				/* Add overflow to high word of ullNextTime. */
			sw t4, 0(t1)				/* Store new low word of ullNextTime. */
			sw t6, 4(t1)				/* Store new high word of ullNextTime. */

		#endif /* __riscv_xlen == 32 */

		#if( __riscv_xlen == 64 )

			/* Update the 64-bit mtimer compare match value. */
			ld t2, 0(t1)			 	/* Load ullNextTime into t2. */
			sd t2, 0(t0)				/* Store ullNextTime into compare register. */
			ld t0, uxTimerIncrementsForOneTick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
			add t4, t0, t2				/* Add ullNextTime to the timer increments for one tick. */
			sd t4, 0(t1)				/* Store ullNextTime. */

		#endif /* __riscv_xlen == 64 */

		LOAD sp, xISRStackTop			/* Switch to ISR stack before function call. */
		jal xTaskIncrementTick
		beqz a0, processed_source		/* Don't switch context if incrementing tick didn't unblock a task. */
		jal vTaskSwitchContext
		j processed_source

	test_if_external_interrupt:			/* If there is a CLINT and the mtimer interrupt is not pending then check to see if an external interrupt is pending. */
		addi t1, t1, 4				/* 0x80000007 + 4 = 0x8000000b == Machine external interrupt. */
	        beq a0, t1, external_interrupt 	/* Could be a machine software interrupt */

        test_if_machine_software_interrupt:
	        addi t1, t0, 3                         /* 0x80000003 == machine software interrupt.*/
	        bne a0, t1, as_yet_unhandled
	        LOAD sp, xISRStackTop
	        jal portasmMSI_HANDLER
	        j processed_source

#endif /* portasmHAS_CLINT */
external_interrupt:
	LOAD sp, xISRStackTop				/* Switch to ISR stack before function call. */
	jal portasmHANDLE_INTERRUPT			/* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that an external interrupt is pending. */
	j processed_source

handle_synchronous:
	addi a1, a1, 4						/* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */
	STORE a1, 0( sp )					/* Save updated exception return address. */

test_if_invalid:
	li t0, portRV_POLICY_VIOLATION_FAULT
	bne a0, t0, test_if_environment_call
   jal end_test_print
   lw t0, test_dev_addr
   lw t1, finish_val
   sw t1, 0(t0)
finish_loop:
   j finish_loop

test_if_environment_call:
	li t0, 11 							/* 11 == environment call. */
	bne a0, t0, is_exception			/* Not an M environment call, so some other exception. */
	LOAD sp, xISRStackTop				/* Switch to ISR stack before function call. */
	jal vTaskSwitchContext
	j processed_source

is_exception:
    /* restore clobbered a0 and x5 */
    LOAD t0, 0(sp)
    LOAD t1, portWORD_SIZE(sp)
    addi sp, sp, 2 * portWORD_SIZE

    // save cpu context for exception printing
    STRUCT_SAVE_CONTEXT
  
    jal is_exception_print

is_exception_loop:
	j is_exception_loop

as_yet_unhandled:
	ebreak
	j as_yet_unhandled

processed_source:
	LOAD  t1, pxCurrentTCB			/* Load pxCurrentTCB. */
	LOAD  sp, 0( t1 )				 	/* Read sp from first TCB member. */

	/* Load mret with the address of the next instruction in the task to run next. */
	LOAD t0, 0( sp )
	csrw mepc, t0

	portasmRESTORE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

	/* Load mstatus with the interrupt enable bits used by the task. */
	LOAD  t0, 29 * portWORD_SIZE( sp )
	csrw mstatus, t0						/* Required for MPIE bit. */

	LOAD  x1, 1 * portWORD_SIZE( sp )
	LOAD  x5, 2 * portWORD_SIZE( sp )		/* t0 */
	LOAD  x6, 3 * portWORD_SIZE( sp )		/* t1 */
	LOAD  x7, 4 * portWORD_SIZE( sp )		/* t2 */
	LOAD  x8, 5 * portWORD_SIZE( sp )		/* s0/fp */
	LOAD  x9, 6 * portWORD_SIZE( sp )		/* s1 */
	LOAD  x10, 7 * portWORD_SIZE( sp )	/* a0 */
	LOAD  x11, 8 * portWORD_SIZE( sp )	/* a1 */
	LOAD  x12, 9 * portWORD_SIZE( sp )	/* a2 */
	LOAD  x13, 10 * portWORD_SIZE( sp )	/* a3 */
	LOAD  x14, 11 * portWORD_SIZE( sp )	/* a4 */
	LOAD  x15, 12 * portWORD_SIZE( sp )	/* a5 */
	LOAD  x16, 13 * portWORD_SIZE( sp )	/* a6 */
	LOAD  x17, 14 * portWORD_SIZE( sp )	/* a7 */
	LOAD  x18, 15 * portWORD_SIZE( sp )	/* s2 */
	LOAD  x19, 16 * portWORD_SIZE( sp )	/* s3 */
	LOAD  x20, 17 * portWORD_SIZE( sp )	/* s4 */
	LOAD  x21, 18 * portWORD_SIZE( sp )	/* s5 */
	LOAD  x22, 19 * portWORD_SIZE( sp )	/* s6 */
	LOAD  x23, 20 * portWORD_SIZE( sp )	/* s7 */
	LOAD  x24, 21 * portWORD_SIZE( sp )	/* s8 */
	LOAD  x25, 22 * portWORD_SIZE( sp )	/* s9 */
	LOAD  x26, 23 * portWORD_SIZE( sp )	/* s10 */
	LOAD  x27, 24 * portWORD_SIZE( sp )	/* s11 */
	LOAD  x28, 25 * portWORD_SIZE( sp )	/* t3 */
	LOAD  x29, 26 * portWORD_SIZE( sp )	/* t4 */
	LOAD  x30, 27 * portWORD_SIZE( sp )	/* t5 */
	LOAD  x31, 28 * portWORD_SIZE( sp )	/* t6 */
	addi sp, sp, portCONTEXT_SIZE

	mret
.size freertos_risc_v_trap_handler, [.-freertos_risc_v_trap_handler]
/*-----------------------------------------------------------*/

.align 8
xPortStartFirstTask:

#if( portasmHAS_CLINT != 0 )
	/* If there is a clint then interrupts can branch directly to the FreeRTOS
	trap handler.  Otherwise the interrupt controller will need to be configured
	outside of this file. */
	la t0, freertos_risc_v_trap_handler
	csrw mtvec, t0
#endif /* portasmHAS_CLINT */

	LOAD  sp, pxCurrentTCB			/* Load pxCurrentTCB. */
	LOAD  sp, 0( sp )				 	/* Read sp from first TCB member. */

	LOAD  x1, 0( sp ) /* Note for starting the scheduler the exception return address is used as the function return address. */

	portasmRESTORE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

	LOAD  t0, 29 * portWORD_SIZE( sp )	/* mstatus */
	csrrw  x0, mstatus, t0					/* Interrupts enabled from here! */

	LOAD  x5, 2 * portWORD_SIZE( sp )		/* t0 */
	LOAD  x6, 3 * portWORD_SIZE( sp )		/* t1 */
	LOAD  x7, 4 * portWORD_SIZE( sp )		/* t2 */
	LOAD  x8, 5 * portWORD_SIZE( sp )		/* s0/fp */
	LOAD  x9, 6 * portWORD_SIZE( sp )		/* s1 */
	LOAD  x10, 7 * portWORD_SIZE( sp )	/* a0 */
	LOAD  x11, 8 * portWORD_SIZE( sp )	/* a1 */
	LOAD  x12, 9 * portWORD_SIZE( sp )	/* a2 */
	LOAD  x13, 10 * portWORD_SIZE( sp )	/* a3 */
	LOAD  x14, 11 * portWORD_SIZE( sp )	/* a4 */
	LOAD  x15, 12 * portWORD_SIZE( sp )	/* a5 */
	LOAD  x16, 13 * portWORD_SIZE( sp )	/* a6 */
	LOAD  x17, 14 * portWORD_SIZE( sp )	/* a7 */
	LOAD  x18, 15 * portWORD_SIZE( sp )	/* s2 */
	LOAD  x19, 16 * portWORD_SIZE( sp )	/* s3 */
	LOAD  x20, 17 * portWORD_SIZE( sp )	/* s4 */
	LOAD  x21, 18 * portWORD_SIZE( sp )	/* s5 */
	LOAD  x22, 19 * portWORD_SIZE( sp )	/* s6 */
	LOAD  x23, 20 * portWORD_SIZE( sp )	/* s7 */
	LOAD  x24, 21 * portWORD_SIZE( sp )	/* s8 */
	LOAD  x25, 22 * portWORD_SIZE( sp )	/* s9 */
	LOAD  x26, 23 * portWORD_SIZE( sp )	/* s10 */
	LOAD  x27, 24 * portWORD_SIZE( sp )	/* s11 */
	LOAD  x28, 25 * portWORD_SIZE( sp )	/* t3 */
	LOAD  x29, 26 * portWORD_SIZE( sp )	/* t4 */
	LOAD  x30, 27 * portWORD_SIZE( sp )	/* t5 */
	LOAD  x31, 28 * portWORD_SIZE( sp )	/* t6 */
	addi	sp, sp, portCONTEXT_SIZE
	ret
/*-----------------------------------------------------------*/

/*
 * Unlike other ports pxPortInitialiseStack() is written in assembly code as it
 * needs access to the portasmADDITIONAL_CONTEXT_SIZE constant.  The prototype
 * for the function is as per the other ports:
 * StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
 *
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2.  The new top of stack is passed out in a0.
 *
 * RISC-V maps registers to ABI names as follows (X1 to X31 integer registers
 * for the 'I' profile, X1 to X15 for the 'E' profile, currently I assumed).
 *
 * Register		ABI Name	Description						Saver
 * x0			zero		Hard-wired zero					-
 * x1			ra			Return address					Caller
 * x2			sp			Stack pointer					Callee
 * x3			gp			Global pointer					-
 * x4			tp			Thread pointer					-
 * x5-7			t0-2		Temporaries						Caller
 * x8			s0/fp		Saved register/Frame pointer	Callee
 * x9			s1			Saved register					Callee
 * x10-11		a0-1		Function Arguments/return values Caller
 * x12-17		a2-7		Function arguments				Caller
 * x18-27		s2-11		Saved registers					Callee
 * x28-31		t3-6		Temporaries						Caller
 *
 * The RISC-V context is saved t FreeRTOS tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * portTASK_RETURN_ADDRESS
 * [chip specific registers go here]
 * pxCode
 */
.align 8
pxPortInitialiseStack:

	csrr t0, mstatus					/* Obtain current mstatus value. */
	addi t1, x0, 0x188					/* Generate the value 0x1880, which are the MPIE and MPP bits to set in mstatus. */
	slli t1, t1, 4
	or t0, t0, t1						/* Set MPIE and MPP bits in mstatus value. */

	addi a0, a0, -portWORD_SIZE
	STORE t0, 0(a0)					/* mstatus onto the stack. */
	addi a0, a0, -(22 * portWORD_SIZE)	/* Space for registers x11-x31. */
	STORE a2, 0(a0)					/* Task parameters (pvParameters parameter) goes into register X10/a0 on the stack. */
	addi a0, a0, -(6 * portWORD_SIZE)	/* Space for registers x5-x9. */
	STORE x0, 0(a0)					/* Return address onto the stack, could be portTASK_RETURN_ADDRESS */
	addi t0, x0, portasmADDITIONAL_CONTEXT_SIZE /* The number of chip specific additional registers. */
chip_specific_stack_frame:				/* First add any chip specific registers to the stack frame being created. */
	beq t0, x0, 1f						/* No more chip specific registers to save. */
	addi a0, a0, -portWORD_SIZE			/* Make space for chip specific register. */
	STORE x0, 0(a0)					/* Give the chip specific register an initial value of zero. */
	addi t0, t0, -1						/* Decrement the count of chip specific registers remaining. */
	j chip_specific_stack_frame			/* Until no more chip specific registers. */
1:
	addi a0, a0, -portWORD_SIZE
	STORE a1, 0(a0)					/* mret value (pxCode parameter) onto the stack. */
	ret
/*-----------------------------------------------------------*/
