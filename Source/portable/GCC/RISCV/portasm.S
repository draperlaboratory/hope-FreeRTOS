/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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
 * RISCV port derived from ARM 7
*/
	
/*#if __riscv_xlen == 64 */
#ifdef __riscv64
# define STORE    sd
# define LOAD     ld
# define REGBYTES 8
#else
# define STORE    sw
# define LOAD     lw
# define REGBYTES 4
#endif

#ifdef PRIV_1_7
#define MSTATUS_PRV1 0x30
#define MSTATUS_MIE  0x01
#else
#define MSTATUS_PRV1 0x1880
#define MSTATUS_MIE  0x08
#define MTIME_BASE      0x4400BFF8
#define MTIMECMP_BASE   0x44004000
	
#endif

	.global vPortInterruptHandler
	.global xPortRawTime
	.global vPortInitTimerInterrupt
	.global vPortSetNextTimerInterrupt
	.global pxPortInitialiseStack
	.global prvSetupTimerInterrupt
    .global xPortStartScheduler
    .global vPortYield
    .global vTaskIncrementTick
    .global vPortEndScheduler
    .global xExitStack
    .global	pxCurrentTCB
    .global vPortDisableInterrupts
    .global vPortEnableInterrupts
    .global prvTaskExitError
	.global debug_status
	
debug_status:
	csrr	a0, mstatus
	ret
	
	/* get 64 bit current time */
xPortRawTime:
	li 		t0, MTIME_BASE
	lw		a1, 4(t0)	/* load high */
	lw		a0, 0(t0)	/* load low  */
	lw		t0, 4(t0)	/* load high */
	bne 	t0, a1, xPortRawTime /* do again if rollover (high changed) */
	ret

	/* Set up the timer first time based on current time */
vPortInitTimerInterrupt:
	li 		t0, MTIME_BASE
	lw		t1, 4(t0)	/* load high */
	lw		t2, 0(t0)	/* load low  */
	lw		t3, 4(t0)	/* load high */
	bne 	t1, t3, vPortInitTimerInterrupt /* do again if rollover (high changed) */
	add		t1, t2,a0	/* 64 bit add */
	sltu	t2, t1,t2
	add		t3, t2,t3
	li 		t0, MTIMECMP_BASE
foobar:	
	sw		t1, 0(t0)	/* update cmp reg */
	sw		t3, 4(t0)
	ret
	
	/* Set up the timer for next time based on last cmp time */
vPortSetNextTimerInterrupt:	
	li 		t0, MTIMECMP_BASE
	lw		t2, 0(t0)	/* load low  */
	lw		t3, 4(t0)	/* load high */
	add		t1, t2,a0	/* 64 bit add */
	sltu	t2, t1,t2
	add		t3, t2,t3
	sw		t1, 0(t0)	/* update cmp reg */
	sw		t3, 4(t0)
	ret

/* Timer interrupt entry point */        
vPortInterruptHandler:
// save context
	/* make room in stack */
	addi	sp, sp, -REGBYTES * 32

	/* Save Registers */
	STORE	x1, 0x0(sp)
	STORE	x2, 1 * REGBYTES(sp)
	STORE	x3, 2 * REGBYTES(sp)
	STORE	x4, 3 * REGBYTES(sp)
	STORE	x5, 4 * REGBYTES(sp)
	STORE	x6, 5 * REGBYTES(sp)
	STORE	x7, 6 * REGBYTES(sp)
	STORE	x8, 7 * REGBYTES(sp)
	STORE	x9, 8 * REGBYTES(sp)
	STORE	x10, 9 * REGBYTES(sp)
	STORE	x11, 10 * REGBYTES(sp)
	STORE	x12, 11 * REGBYTES(sp)
	STORE	x13, 12 * REGBYTES(sp)
	STORE	x14, 13 * REGBYTES(sp)
	STORE	x15, 14 * REGBYTES(sp)
	STORE	x16, 15 * REGBYTES(sp)
	STORE	x17, 16 * REGBYTES(sp)
	STORE	x18, 17 * REGBYTES(sp)
	STORE	x19, 18 * REGBYTES(sp)
	STORE	x20, 19 * REGBYTES(sp)
	STORE	x21, 20 * REGBYTES(sp)
	STORE	x22, 21 * REGBYTES(sp)
	STORE	x23, 22 * REGBYTES(sp)
	STORE	x24, 23 * REGBYTES(sp)
	STORE	x25, 24 * REGBYTES(sp)
	STORE	x26, 25 * REGBYTES(sp)
	STORE	x27, 26 * REGBYTES(sp)
	STORE	x28, 27 * REGBYTES(sp)
	STORE	x29, 28 * REGBYTES(sp)
	STORE	x30, 29 * REGBYTES(sp)
	STORE	x31, 30 * REGBYTES(sp)
	
	/* Store current stackpointer in task control block (TCB) */
	LOAD	t0, pxCurrentTCB
	STORE	sp, 0x0(t0)

        /* Save epc as future pc on stack */
  	csrrw	t0, mepc, t0
	STORE	t0, 31 * REGBYTES(sp)
        /* Call scheduler to possibly switch tasks by changing TCB ptr */
	jal	vPortSysTickHandler
	/* Load stack pointer from the current TCB */
	LOAD	sp, pxCurrentTCB
	LOAD	sp, 0x0(sp)
	/* Return to task in machine mode */
  	li 	t0, MSTATUS_PRV1
        csrs	mstatus, t0
        /* restore saved pc to epc for mret back to task */
	LOAD	t0, 31 * REGBYTES(sp)
  	csrrw	t0, mepc, t0
	/* Restore registers */
	LOAD	x1, 0x0(sp)
        /* skip sp */
	LOAD	x3, 2 * REGBYTES(sp)
	LOAD	x4, 3 * REGBYTES(sp)
	LOAD	x5, 4 * REGBYTES(sp)
	LOAD	x6, 5 * REGBYTES(sp)
	LOAD	x7, 6 * REGBYTES(sp)
	LOAD	x8, 7 * REGBYTES(sp)
	LOAD	x9, 8 * REGBYTES(sp)
	LOAD	x10, 9 * REGBYTES(sp)
	LOAD	x11, 10 * REGBYTES(sp)
	LOAD	x12, 11 * REGBYTES(sp)
	LOAD	x13, 12 * REGBYTES(sp)
	LOAD	x14, 13 * REGBYTES(sp)
	LOAD	x15, 14 * REGBYTES(sp)
	LOAD	x16, 15 * REGBYTES(sp)
	LOAD	x17, 16 * REGBYTES(sp)
	LOAD	x18, 17 * REGBYTES(sp)
	LOAD	x19, 18 * REGBYTES(sp)
	LOAD	x20, 19 * REGBYTES(sp)
	LOAD	x21, 20 * REGBYTES(sp)
	LOAD	x22, 21 * REGBYTES(sp)
	LOAD	x23, 22 * REGBYTES(sp)
	LOAD	x24, 23 * REGBYTES(sp)
	LOAD	x25, 24 * REGBYTES(sp)
	LOAD	x26, 25 * REGBYTES(sp)
	LOAD	x27, 26 * REGBYTES(sp)
	LOAD	x28, 27 * REGBYTES(sp)
	LOAD	x29, 28 * REGBYTES(sp)
	LOAD	x30, 29 * REGBYTES(sp)
	LOAD	x31, 30 * REGBYTES(sp)
        /* pop stack */
	addi	sp, sp, REGBYTES * 32
        /* return via epc address to task */
	mret

/* initialize the stack before starting a task
 * so that when scheduled for first time the return
 * goes to the beginning of the task
 */
pxPortInitialiseStack:
        /* Push the stack to make room for save regs */
	addi	a0, a0, -32 * REGBYTES
        /* save task pointer as future pc */
	STORE 	a1, 31 * REGBYTES(a0)
        /* save task parameters in what will be a0 */
	STORE 	a2, 9 * REGBYTES(a0)
        /* save gp in what will be gp */
	STORE 	gp, 2 * REGBYTES(a0)
        /* set return address to error function (tasks never return) */
	la	a2, prvTaskExitError
	STORE 	a2, 0 * REGBYTES(a0)
	ret
	
xPortStartScheduler:
#ifdef __gracefulExit
	/* Stores context when starting the scheduler in xStartContext.
	   This is used for when you want to gracefully exit the scheduler.
	   For example if you want to test multiple instances after each other in one test suite.
     */
	la		t0, xStartContext
	STORE	x1, 0x0(t0)
	STORE	x2, 1 * REGBYTES(t0)
	STORE	x3, 2 * REGBYTES(t0)
	STORE	x4, 3 * REGBYTES(t0)
	STORE	x5, 4 * REGBYTES(t0)
	STORE	x6, 5 * REGBYTES(t0)
	STORE	x7, 6 * REGBYTES(t0)
	STORE	x8, 7 * REGBYTES(t0)
	STORE	x9, 8 * REGBYTES(t0)
	STORE	x10, 9 * REGBYTES(t0)
	STORE	x11, 10 * REGBYTES(t0)
	STORE	x12, 11 * REGBYTES(t0)
	STORE	x13, 12 * REGBYTES(t0)
	STORE	x14, 13 * REGBYTES(t0)
	STORE	x15, 14 * REGBYTES(t0)
	STORE	x16, 15 * REGBYTES(t0)
	STORE	x17, 16 * REGBYTES(t0)
	STORE	x18, 17 * REGBYTES(t0)
	STORE	x19, 18 * REGBYTES(t0)
	STORE	x20, 19 * REGBYTES(t0)
	STORE	x21, 20 * REGBYTES(t0)
	STORE	x22, 21 * REGBYTES(t0)
	STORE	x23, 22 * REGBYTES(t0)
	STORE	x24, 23 * REGBYTES(t0)
	STORE	x25, 24 * REGBYTES(t0)
	STORE	x26, 25 * REGBYTES(t0)
	STORE	x27, 26 * REGBYTES(t0)
	STORE	x28, 27 * REGBYTES(t0)
	STORE	x29, 28 * REGBYTES(t0)
	STORE	x30, 29 * REGBYTES(t0)
	STORE	x31, 30 * REGBYTES(t0)
#endif
        /* setup for first interrupt */
	jal		prvSetupTimerInterrupt
	/* Load stack pointer from the current TCB */
	LOAD	sp, pxCurrentTCB
	LOAD	sp, 0x0(sp)
	/* Ensure we run tasks in machine mode */
  	li 		t0, MSTATUS_PRV1
  	csrs	mstatus, t0
        /* restore saved pc to epc for mret back to task */
	LOAD	t0, 31 * REGBYTES(sp)
  	csrrw	t0, mepc, t0
	/* Restore registers */
	LOAD	x1, 0x0(sp)
        /* skip sp */
	LOAD	x3, 2 * REGBYTES(sp)
	LOAD	x4, 3 * REGBYTES(sp)
	LOAD	x5, 4 * REGBYTES(sp)
	LOAD	x6, 5 * REGBYTES(sp)
	LOAD	x7, 6 * REGBYTES(sp)
	LOAD	x8, 7 * REGBYTES(sp)
	LOAD	x9, 8 * REGBYTES(sp)
	LOAD	x10, 9 * REGBYTES(sp)
	LOAD	x11, 10 * REGBYTES(sp)
	LOAD	x12, 11 * REGBYTES(sp)
	LOAD	x13, 12 * REGBYTES(sp)
	LOAD	x14, 13 * REGBYTES(sp)
	LOAD	x15, 14 * REGBYTES(sp)
	LOAD	x16, 15 * REGBYTES(sp)
	LOAD	x17, 16 * REGBYTES(sp)
	LOAD	x18, 17 * REGBYTES(sp)
	LOAD	x19, 18 * REGBYTES(sp)
	LOAD	x20, 19 * REGBYTES(sp)
	LOAD	x21, 20 * REGBYTES(sp)
	LOAD	x22, 21 * REGBYTES(sp)
	LOAD	x23, 22 * REGBYTES(sp)
	LOAD	x24, 23 * REGBYTES(sp)
	LOAD	x25, 24 * REGBYTES(sp)
	LOAD	x26, 25 * REGBYTES(sp)
	LOAD	x27, 26 * REGBYTES(sp)
	LOAD	x28, 27 * REGBYTES(sp)
	LOAD	x29, 28 * REGBYTES(sp)
	LOAD	x30, 29 * REGBYTES(sp)
	LOAD	x31, 30 * REGBYTES(sp)

        /* pop stack */
	addi	sp, sp, REGBYTES * 32
        /* return via epc address to task */
	mret

/* terminate scheduler and restore context */
vPortEndScheduler:

#ifdef __gracefulExit
	/* Load current context from xStartContext */
	la	t0, xStartContext
	LOAD	x1, 0x0(t0)
	LOAD	x2, 1 * REGBYTES(t0)
	LOAD	x3, 2 * REGBYTES(t0)
	LOAD	x4, 3 * REGBYTES(t0)
	LOAD	x5, 4 * REGBYTES(t0)
	LOAD	x6, 5 * REGBYTES(t0)
	LOAD	x7, 6 * REGBYTES(t0)
	LOAD	x8, 7 * REGBYTES(t0)
	LOAD	x9, 8 * REGBYTES(t0)
	LOAD	x10, 9 * REGBYTES(t0)
	LOAD	x11, 10 * REGBYTES(t0)
	LOAD	x12, 11 * REGBYTES(t0)
	LOAD	x13, 12 * REGBYTES(t0)
	LOAD	x14, 13 * REGBYTES(t0)
	LOAD	x15, 14 * REGBYTES(t0)
	LOAD	x16, 15 * REGBYTES(t0)
	LOAD	x17, 16 * REGBYTES(t0)
	LOAD	x18, 17 * REGBYTES(t0)
	LOAD	x19, 18 * REGBYTES(t0)
	LOAD	x20, 19 * REGBYTES(t0)
	LOAD	x21, 20 * REGBYTES(t0)
	LOAD	x22, 21 * REGBYTES(t0)
	LOAD	x23, 22 * REGBYTES(t0)
	LOAD	x24, 23 * REGBYTES(t0)
	LOAD	x25, 24 * REGBYTES(t0)
	LOAD	x26, 25 * REGBYTES(t0)
	LOAD	x27, 26 * REGBYTES(t0)
	LOAD	x28, 27 * REGBYTES(t0)
	LOAD	x29, 28 * REGBYTES(t0)
	LOAD	x30, 39 * REGBYTES(t0)
	LOAD	x31, 30 * REGBYTES(t0)
#endif
	ret

/* yield called by tasks to return control to scheduler */
vPortYield:
        /* disable interrupts in case called outside critical section */
        csrci mstatus, MSTATUS_MIE
	/* make room in stack */
	addi	sp, sp, -REGBYTES * 32

	/* Save Registers */
	STORE	x1, 0x0(sp)
	STORE	x2, 1 * REGBYTES(sp)
	STORE	x3, 2 * REGBYTES(sp)
	STORE	x4, 3 * REGBYTES(sp)
	STORE	x5, 4 * REGBYTES(sp)
	STORE	x6, 5 * REGBYTES(sp)
	STORE	x7, 6 * REGBYTES(sp)
	STORE	x8, 7 * REGBYTES(sp)
	STORE	x9, 8 * REGBYTES(sp)
	STORE	x10, 9 * REGBYTES(sp)
	STORE	x11, 10 * REGBYTES(sp)
	STORE	x12, 11 * REGBYTES(sp)
	STORE	x13, 12 * REGBYTES(sp)
	STORE	x14, 13 * REGBYTES(sp)
	STORE	x15, 14 * REGBYTES(sp)
	STORE	x16, 15 * REGBYTES(sp)
	STORE	x17, 16 * REGBYTES(sp)
	STORE	x18, 17 * REGBYTES(sp)
	STORE	x19, 18 * REGBYTES(sp)
	STORE	x20, 19 * REGBYTES(sp)
	STORE	x21, 20 * REGBYTES(sp)
	STORE	x22, 21 * REGBYTES(sp)
	STORE	x23, 22 * REGBYTES(sp)
	STORE	x24, 23 * REGBYTES(sp)
	STORE	x25, 24 * REGBYTES(sp)
	STORE	x26, 25 * REGBYTES(sp)
	STORE	x27, 26 * REGBYTES(sp)
	STORE	x28, 27 * REGBYTES(sp)
	STORE	x29, 28 * REGBYTES(sp)
	STORE	x30, 29 * REGBYTES(sp)
	STORE	x31, 30 * REGBYTES(sp)
	
	/* Store current stackpointer in task control block (TCB) */
	LOAD	t0, pxCurrentTCB
	STORE	sp, 0x0(t0)
        /* Save return address as future pc on stack */
	STORE	ra, 31 * REGBYTES(sp)
        /* Call scheduler to possibly switch tasks by changing TCB ptr */
	jal	vTaskSwitchContext
        //        portRestore_STACK
	/* Load stack pointer from the current TCB */
	LOAD	sp, pxCurrentTCB
	LOAD	sp, 0x0(sp)
	/* Ensure we run tasks in machine mode */
  	li 	t0, MSTATUS_PRV1
  	csrs	mstatus, t0
        
        /* restore saved pc to epc for mret back to task */
	LOAD	t0, 31 * REGBYTES(sp)
  	csrrw	t0, mepc, t0

	/* Restore registers */
	LOAD	x1, 0x0(sp)
        /* skip sp */
	LOAD	x3, 2 * REGBYTES(sp)
	LOAD	x4, 3 * REGBYTES(sp)
	LOAD	x5, 4 * REGBYTES(sp)
	LOAD	x6, 5 * REGBYTES(sp)
	LOAD	x7, 6 * REGBYTES(sp)
	LOAD	x8, 7 * REGBYTES(sp)
	LOAD	x9, 8 * REGBYTES(sp)
	LOAD	x10, 9 * REGBYTES(sp)
	LOAD	x11, 10 * REGBYTES(sp)
	LOAD	x12, 11 * REGBYTES(sp)
	LOAD	x13, 12 * REGBYTES(sp)
	LOAD	x14, 13 * REGBYTES(sp)
	LOAD	x15, 14 * REGBYTES(sp)
	LOAD	x16, 15 * REGBYTES(sp)
	LOAD	x17, 16 * REGBYTES(sp)
	LOAD	x18, 17 * REGBYTES(sp)
	LOAD	x19, 18 * REGBYTES(sp)
	LOAD	x20, 19 * REGBYTES(sp)
	LOAD	x21, 20 * REGBYTES(sp)
	LOAD	x22, 21 * REGBYTES(sp)
	LOAD	x23, 22 * REGBYTES(sp)
	LOAD	x24, 23 * REGBYTES(sp)
	LOAD	x25, 24 * REGBYTES(sp)
	LOAD	x26, 25 * REGBYTES(sp)
	LOAD	x27, 26 * REGBYTES(sp)
	LOAD	x28, 27 * REGBYTES(sp)
	LOAD	x29, 28 * REGBYTES(sp)
	LOAD	x30, 29 * REGBYTES(sp)
	LOAD	x31, 30 * REGBYTES(sp)

        /* pop stack */
	addi	sp, sp, REGBYTES * 32
        /* return via epc address to task */
	mret


vPortDisableInterrupts:
        csrci mstatus, MSTATUS_MIE
        ret

vPortEnableInterrupts:
    csrsi mstatus, MSTATUS_MIE
    ret
