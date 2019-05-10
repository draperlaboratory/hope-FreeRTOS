/*
 * FreeRTOS Kernel V10.2.0
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

#if __riscv_xlen == 64
	#error Not implemented yet - change lw to ld, and sw to sd.
	#define portWORD_SIZE 8
#elif __riscv_xlen == 32
	#define portWORD_SIZE 4
#else
	#error Assembler did not define __riscv_xlen
#endif

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
#include "freertos_risc_v_chip_specific_extensions.h"

/* Check the freertos_risc_v_chip_specific_extensions.h and/or command line
definitions. */
#ifndef portasmHAS_CLINT
	#error freertos_risc_v_chip_specific_extensions.h must define portasmHAS_CLINT to either 1 (CLINT present) or 0 (clint not present).
#endif

#ifndef portasmHANDLE_INTERRUPT
	#error portasmHANDLE_INTERRUPT must be defined to the function to be called to handle external/peripheral interrupts.  portasmHANDLE_INTERRUPT can be defined on the assmbler command line or in the appropriate freertos_risc_v_chip_specific_extensions.h header file.
#endif

/* Only the standard core registers are stored by default.  Any additional
registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
specific version of freertos_risc_v_chip_specific_extensions.h.  See the notes
at the top of this file. */
#define portCONTEXT_SIZE ( 30 * portWORD_SIZE )

#define MC_MSTATUS_OFF 32
#define MC_MTVEC_OFF 33
#define MC_MIP_OFF 34
#define MC_MIE_OFF 35
#define MC_MEPC_OFF 36
#define MC_MCAUSE_OFF 37
#define MC_MTVAL_OFF 38
#define MC_MISA_OFF 39
  
.global xPortStartFirstTask
.global freertos_risc_v_trap_handler
.global pxPortInitialiseStack
.extern pxCurrentTCB
.extern ulPortTrapHandler
.extern vTaskSwitchContext
.extern Timer_IRQHandler
.extern pullMachineTimerCompareRegister
.extern pullNextTime
.extern ulTimerIncrementsForOneTick
.extern xISRStackTop

.extern global_exception_mc

/*-----------------------------------------------------------*/

/* save cpu context to structure */	
.macro STRUCT_SAVE_CONTEXT

  // store a0 and x5 for use
  addi sp, sp, -2 * portWORD_SIZE
  sw a0, 0(sp)
  sw x5, portWORD_SIZE(sp)
  
  la a0, global_exception_mc
  
  sw x1, 1 * portWORD_SIZE( a0 )

  // x2 is sp, get regular sp back to save
  addi sp, sp, 2 * portWORD_SIZE
  sw x2, 2 * portWORD_SIZE( a0 )
  addi sp, sp, -2 * portWORD_SIZE
  
  sw x3, 3 * portWORD_SIZE( a0 )
  sw x4, 4 * portWORD_SIZE( a0 )
  sw x5, 5 * portWORD_SIZE( a0 )
  sw x6, 6 * portWORD_SIZE( a0 )
  sw x7, 7 * portWORD_SIZE( a0 )
  sw x8, 8 * portWORD_SIZE( a0 )
  sw x9, 9 * portWORD_SIZE( a0 )

  // instead of	sw x10, 10 * portWORD_SIZE( a0 )
  lw x5, 0(sp)
  sw x5, 10 * portWORD_SIZE( a0 )
  
  sw x11, 11 * portWORD_SIZE( a0 )
  sw x12, 12 * portWORD_SIZE( a0 )
  sw x13, 13 * portWORD_SIZE( a0 )
  sw x14, 14 * portWORD_SIZE( a0 )
  sw x15, 15 * portWORD_SIZE( a0 )
  sw x16, 16 * portWORD_SIZE( a0 )
  sw x17, 17 * portWORD_SIZE( a0 )
  sw x18, 18 * portWORD_SIZE( a0 )
  sw x19, 19 * portWORD_SIZE( a0 )
  sw x20, 20 * portWORD_SIZE( a0 )
  sw x21, 21 * portWORD_SIZE( a0 )
  sw x22, 22 * portWORD_SIZE( a0 )
  sw x23, 23 * portWORD_SIZE( a0 )
  sw x24, 24 * portWORD_SIZE( a0 )
  sw x25, 25 * portWORD_SIZE( a0 )
  sw x26, 26 * portWORD_SIZE( a0 )
  sw x27, 27 * portWORD_SIZE( a0 )
  sw x28, 28 * portWORD_SIZE( a0 )
  sw x29, 29 * portWORD_SIZE( a0 )
  sw x30, 30 * portWORD_SIZE( a0 )
  sw x31, 31 * portWORD_SIZE( a0 )

	// save CSRs
  csrr x5, mstatus
  sw x5, MC_MSTATUS_OFF * portWORD_SIZE( a0 )

  csrr x5, mtvec
  sw x5, MC_MTVEC_OFF * portWORD_SIZE( a0 )

  csrr x5, mip
  sw x5, MC_MIP_OFF * portWORD_SIZE( a0 )

  csrr x5, mie
  sw x5, MC_MIE_OFF * portWORD_SIZE( a0 )

  csrr x5, mepc
  sw x5, MC_MEPC_OFF * portWORD_SIZE( a0 )

  csrr x5, mcause
  sw x5, MC_MCAUSE_OFF * portWORD_SIZE( a0 )

  csrr x5, mtval
  sw x5, MC_MTVAL_OFF * portWORD_SIZE( a0 )

  csrr x5, misa
  sw x5, MC_MISA_OFF * portWORD_SIZE( a0 )
  
  /* restore clobbered a0 and x5 */
  lw a0, 0(sp)
  lw x5, portWORD_SIZE(sp)       
  addi sp, sp, 2 * portWORD_SIZE
  
.endm

.align 8
freertos_risc_v_trap_handler:

  /* check for exception */
  /* save scratch regs */
	addi sp, sp, -2 * portWORD_SIZE
  sw t0, 0(sp)
  sw t1, portWORD_SIZE(sp)

  csrr t1, mcause
  
  // 0x0 - instruction addr misaligned
  li t0, 0x0
  beq t0, t1, jump_to_is_exception
  
  // 0x1 - instruction access fault
  li t0, 0x1
  beq t0, t1, jump_to_is_exception
  
  // 0x2 - illegal instruction
  li t0, 0x2
  beq t0, t1, jump_to_is_exception
  
  // 0x4 - load address misaligned
  li t0, 0x4
  beq t0, t1, jump_to_is_exception
  
  // 0x5 - load access fault
  li t0, 0x5
  beq t0, t1, jump_to_is_exception
  
  // 0x6 - store/AMO addr misaligned
  li t0, 0x6
  beq t0, t1, jump_to_is_exception
  
  // 0x7 - store/AMO access fualt
  li t0, 0x7
  beq t0, t1, jump_to_is_exception
  
  // 0x12 - instruction page fault
  li t0, 0x12
  beq t0, t1, jump_to_is_exception
  
  // 0x13 - load page fault
  li t0, 0x13
  beq t0, t1, jump_to_is_exception
  
  // 0x15 - store/AMO page fault
  li t0, 0x15
  beq t0, t1, jump_to_is_exception

  j no_exception
  
jump_to_is_exception: 
  /* restore clobbered a0 and x5 */
  lw t0, 0(sp)
  lw t1, portWORD_SIZE(sp)       
  addi sp, sp, 2 * portWORD_SIZE

  j is_exception

no_exception: 

	/* restore clobbered a0 and x5 */
  lw t0, 0(sp)
  lw t1, portWORD_SIZE(sp)       
  addi sp, sp, 2 * portWORD_SIZE

	/* beginning of regular trap handler. */
  addi sp, sp, -portCONTEXT_SIZE
  sw x1, 1 * portWORD_SIZE( sp )
  sw x5, 2 * portWORD_SIZE( sp )
  sw x6, 3 * portWORD_SIZE( sp )
  sw x7, 4 * portWORD_SIZE( sp )
  sw x8, 5 * portWORD_SIZE( sp )
  sw x9, 6 * portWORD_SIZE( sp )
  sw x10, 7 * portWORD_SIZE( sp )
  sw x11, 8 * portWORD_SIZE( sp )
  sw x12, 9 * portWORD_SIZE( sp )
  sw x13, 10 * portWORD_SIZE( sp )
  sw x14, 11 * portWORD_SIZE( sp )
  sw x15, 12 * portWORD_SIZE( sp )
  sw x16, 13 * portWORD_SIZE( sp )
  sw x17, 14 * portWORD_SIZE( sp )
  sw x18, 15 * portWORD_SIZE( sp )
  sw x19, 16 * portWORD_SIZE( sp )
  sw x20, 17 * portWORD_SIZE( sp )
  sw x21, 18 * portWORD_SIZE( sp )
  sw x22, 19 * portWORD_SIZE( sp )
  sw x23, 20 * portWORD_SIZE( sp )
  sw x24, 21 * portWORD_SIZE( sp )
  sw x25, 22 * portWORD_SIZE( sp )
  sw x26, 23 * portWORD_SIZE( sp )
  sw x27, 24 * portWORD_SIZE( sp )
  sw x28, 25 * portWORD_SIZE( sp )
  sw x29, 26 * portWORD_SIZE( sp )
  sw x30, 27 * portWORD_SIZE( sp )
  sw x31, 28 * portWORD_SIZE( sp )

  csrr t0, mstatus					/* Required for MPIE bit. */
  sw t0, 29 * portWORD_SIZE( sp )

  portasmSAVE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

  lw  t0, pxCurrentTCB				/* Load pxCurrentTCB. */
  sw  sp, 0( t0 )				 		/* Write sp to first TCB member. */

  csrr a0, mcause
  csrr a1, mepc

test_if_asynchronous:
  srli a2, a0, 0x1f					/* MSB of mcause is 1 if handing an asynchronous interrupt - shift to LSB to clear other bits. */
  beq a2, x0, handle_synchronous		/* Branch past interrupt handing if not asynchronous. */
  sw a1, 0( sp )						/* Asynch so save unmodified exception return address. */

handle_asynchronous:

#if( portasmHAS_CLINT != 0 )

test_if_mtimer:							/* If there is a CLINT then the mtimer is used to generate the tick interrupt. */
  lui t0, 0x80000
  addi t1, t0, 7						/* 0x80000007 == machine timer interrupt. */
  bne a0, t1, test_if_external_interrupt

  lw t0, pullMachineTimerCompareRegister	/* Load address of compare register into t0. */
		lw t1, pullNextTime					/* Load the address of ullNextTime into t1. */
		lw t2, 0(t1)						/* Load the low word of ullNextTime into t2. */
		lw t3, 4(t1)						/* Load the high word of ullNextTime into t3. */
		sw t2, 0(t0)						/* Store low word of ullNextTime into compare register. */
		sw t3, 4(t0)						/* Store high word of ullNextTime into compare register. */
		lw t0, ulTimerIncrementsForOneTick	/* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
		add t4, t0, t2						/* Add the low word of ullNextTime to the timer increments for one tick (assumes timer increment for one tick fits in 32-bits. */
		sltu t5, t4, t2						/* See if the sum of low words overflowed (what about the zero case?). */
		add t6, t3, t5						/* Add overflow to high word of ullNextTime. */
		sw t4, 0(t1)						/* Store new low word of ullNextTime. */
		sw t6, 4(t1)						/* Store new high word of ullNextTime. */
		lw sp, xISRStackTop					/* Switch to ISR stack before function call. */
		jal xTaskIncrementTick
		beqz a0, processed_source			/* Don't switch context if incrementing tick didn't unblock a task. */
		jal vTaskSwitchContext
		j processed_source

	test_if_external_interrupt:			/* If there is a CLINT and the mtimer interrupt is not pending then check to see if an external interrupt is pending. */
		addi t1, t1, 4					/* 0x80000007 + 4 = 0x8000000b == Machine external interrupt. */
		bne a0, t1, as_yet_unhandled	/* Something as yet unhandled. */

#endif /* portasmHAS_CLINT */

	lw sp, xISRStackTop					/* Switch to ISR stack before function call. */
	jal portasmHANDLE_INTERRUPT			/* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that an external interrupt is pending. */
	j processed_source

handle_synchronous:
	addi a1, a1, 4						/* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */
	sw a1, 0( sp )						/* Save updated exception return address. */

test_if_access_fault:
	li t0, 5 							   /* 5 == Load Access Fault */
	beq a0, t0, handle_access_fault
   li t0, 7 							   /* 7 == Store Access Fault */
	beq a0, t0, handle_access_fault

test_if_environment_call:
	li t0, 11 							/* 11 == environment call. */
	bne a0, t0, is_exception			/* Not an M environment call, so some other exception. */
	lw sp, xISRStackTop					/* Switch to ISR stack before function call. */
	jal vTaskSwitchContext
	j processed_source

handle_access_fault:
	j test_if_environment_call

is_exception:

	// save cpu context for exception printing
  STRUCT_SAVE_CONTEXT
  
  jal is_exception_print
	
	ebreak
is_exception_loop:
	j is_exception_loop

as_yet_unhandled:
	ebreak
	j as_yet_unhandled

processed_source:
	lw  sp, pxCurrentTCB				/* Load pxCurrentTCB. */
	lw  sp, 0( sp )				  		/* Read sp from first TCB member. */

	/* Load mret with the address of the next instruction in the task to run next. */
	lw t0, 0( sp )
	csrw mepc, t0

	portasmRESTORE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

	/* Load mstatus with the interrupt enable bits used by the task. */
	lw  t0, 29 * portWORD_SIZE( sp )
	csrw mstatus, t0					/* Required for MPIE bit. */

	lw  x1, 1 * portWORD_SIZE( sp )
	lw  x5, 2 * portWORD_SIZE( sp )		/* t0 */
	lw  x6, 3 * portWORD_SIZE( sp )		/* t1 */
	lw  x7, 4 * portWORD_SIZE( sp )		/* t2 */
	lw  x8, 5 * portWORD_SIZE( sp )		/* s0/fp */
	lw  x9, 6 * portWORD_SIZE( sp )		/* s1 */
	lw  x10, 7 * portWORD_SIZE( sp )	/* a0 */
	lw  x11, 8 * portWORD_SIZE( sp )	/* a1 */
	lw  x12, 9 * portWORD_SIZE( sp )	/* a2 */
	lw  x13, 10 * portWORD_SIZE( sp )	/* a3 */
	lw  x14, 11 * portWORD_SIZE( sp )	/* a4 */
	lw  x15, 12 * portWORD_SIZE( sp )	/* a5 */
	lw  x16, 13 * portWORD_SIZE( sp )	/* a6 */
	lw  x17, 14 * portWORD_SIZE( sp )	/* a7 */
	lw  x18, 15 * portWORD_SIZE( sp )	/* s2 */
	lw  x19, 16 * portWORD_SIZE( sp )	/* s3 */
	lw  x20, 17 * portWORD_SIZE( sp )	/* s4 */
	lw  x21, 18 * portWORD_SIZE( sp )	/* s5 */
	lw  x22, 19 * portWORD_SIZE( sp )	/* s6 */
	lw  x23, 20 * portWORD_SIZE( sp )	/* s7 */
	lw  x24, 21 * portWORD_SIZE( sp )	/* s8 */
	lw  x25, 22 * portWORD_SIZE( sp )	/* s9 */
	lw  x26, 23 * portWORD_SIZE( sp )	/* s10 */
	lw  x27, 24 * portWORD_SIZE( sp )	/* s11 */
	lw  x28, 25 * portWORD_SIZE( sp )	/* t3 */
	lw  x29, 26 * portWORD_SIZE( sp )	/* t4 */
	lw  x30, 27 * portWORD_SIZE( sp )	/* t5 */
	lw  x31, 28 * portWORD_SIZE( sp )	/* t6 */
	addi sp, sp, portCONTEXT_SIZE

	mret
/*-----------------------------------------------------------*/

.align 8
xPortStartFirstTask:

#if( portasmHAS_CLINT != 0 )
	/* If there is a clint then interrupts can branch directly to the FreeRTOS
	trap handler.  Otherwise the interrupt controller will need to be configured
	outside of this file. */
	la t0, freertos_risc_v_trap_handler
	csrw mtvec, t0
#endif /* portasmHAS_CLILNT */

	lw  sp, pxCurrentTCB			/* Load pxCurrentTCB. */
	lw  sp, 0( sp )				 	/* Read sp from first TCB member. */

	lw  x1, 0( sp ) /* Note for starting the scheduler the exception return address is used as the function return address. */

	portasmRESTORE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

    lw  t0, 29 * portWORD_SIZE( sp )    /* mstatus */
    csrrw  x0, mstatus, t0              /* Interrupts enabled from here! */

	lw  x5, 2 * portWORD_SIZE( sp )		/* t0 */
	lw  x6, 3 * portWORD_SIZE( sp )		/* t1 */
	lw  x7, 4 * portWORD_SIZE( sp )		/* t2 */
	lw  x8, 5 * portWORD_SIZE( sp )		/* s0/fp */
	lw  x9, 6 * portWORD_SIZE( sp )		/* s1 */
	lw  x10, 7 * portWORD_SIZE( sp )	/* a0 */
	lw  x11, 8 * portWORD_SIZE( sp )	/* a1 */
	lw  x12, 9 * portWORD_SIZE( sp )	/* a2 */
	lw  x13, 10 * portWORD_SIZE( sp )	/* a3 */
	lw  x14, 11 * portWORD_SIZE( sp )	/* a4 */
	lw  x15, 12 * portWORD_SIZE( sp )	/* a5 */
	lw  x16, 13 * portWORD_SIZE( sp )	/* a6 */
	lw  x17, 14 * portWORD_SIZE( sp )	/* a7 */
	lw  x18, 15 * portWORD_SIZE( sp )	/* s2 */
	lw  x19, 16 * portWORD_SIZE( sp )	/* s3 */
	lw  x20, 17 * portWORD_SIZE( sp )	/* s4 */
	lw  x21, 18 * portWORD_SIZE( sp )	/* s5 */
	lw  x22, 19 * portWORD_SIZE( sp )	/* s6 */
	lw  x23, 20 * portWORD_SIZE( sp )	/* s7 */
	lw  x24, 21 * portWORD_SIZE( sp )	/* s8 */
	lw  x25, 22 * portWORD_SIZE( sp )	/* s9 */
	lw  x26, 23 * portWORD_SIZE( sp )	/* s10 */
	lw  x27, 24 * portWORD_SIZE( sp )	/* s11 */
	lw  x28, 25 * portWORD_SIZE( sp )	/* t3 */
	lw  x29, 26 * portWORD_SIZE( sp )	/* t4 */
	lw  x30, 27 * portWORD_SIZE( sp )	/* t5 */
	lw  x31, 28 * portWORD_SIZE( sp )	/* t6 */
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
	sw t0, 0(a0)						/* mstatus onto the stack. */
	addi a0, a0, -(22 * portWORD_SIZE)	/* Space for registers x11-x31. */
	sw a2, 0(a0)						/* Task parameters (pvParameters parameter) goes into register X10/a0 on the stack. */
	addi a0, a0, -(6 * portWORD_SIZE)	/* Space for registers x5-x9. */
	sw x0, 0(a0)						/* Return address onto the stack, could be portTASK_RETURN_ADDRESS */
	addi t0, x0, portasmADDITIONAL_CONTEXT_SIZE /* The number of chip specific additional registers. */
chip_specific_stack_frame:				/* First add any chip specific registers to the stack frame being created. */
	beq t0, x0, 1f						/* No more chip specific registers to save. */
	addi a0, a0, -portWORD_SIZE			/* Make space for chip specific register. */
	sw x0, 0(a0)						/* Give the chip specific register an initial value of zero. */
	addi t0, t0, -1						/* Decrement the count of chip specific registers remaining. */
	j chip_specific_stack_frame			/* Until no more chip specific registers. */
1:
	addi a0, a0, -portWORD_SIZE
	sw a1, 0(a0)						/* mret value (pxCode parameter) onto the stack. */
	ret
/*-----------------------------------------------------------*/
