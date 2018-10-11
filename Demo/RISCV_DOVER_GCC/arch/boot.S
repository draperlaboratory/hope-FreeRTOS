/*
 * Copyright © 2017-2018 Dover Microsystems, Inc.
 * All rights reserved. 
 *
 * Use and disclosure subject to the following license. 
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

#define MSTATUS_PRV1 0x1880
#define MSTATUS_MIE  0x08
#define MIE_MTIE     0x80
#define MIP_MTIP     0x80
#define MTIME_BASE      0x4400BFF8
#define MTIMECMP_BASE   0x44004000
	

	.global main
	.global vPortInterruptHandler
	.global ulSyscallTrap
	.global prvTaskExitError
	.global debug_mstatus
	.global debug_mie
	.global debug_mip
	.global debug_mtvec
	
.section .text,"ax",@progbits
	.align 6
user_trap_entry:
	j	trap_entry

	.align 6
supervisor_trap_entry:
	j	trap_entry

	.align 6
hypervisor_trap_entry:
	j	trap_entry

	.align 6
machine_trap_entry:
	j	trap_entry


	.align 6
	.section .text,"ax",@progbits
	.globl _mstart

boot:
    j _mstart

	.align 6
	.section .text,"ax",@progbits
	.globl _mstart

/* Startup code */
_mstart:
/* setup mtvec for trap vector */
    //la t0, trap_entry
    lui t0, %hi( trap_entry)
    addi t0, t0, %lo( trap_entry)
    csrw mtvec, t0
	/* set mstatus for mode */
    li t6, 0x1800
    csrw mstatus, t6
	/* enble timer ints */
	li		t0, MIE_MTIE
    csrw 	mie, t0

	
    li	x1, 0
    li	x2, 0
    li	x3, 0
    li	x4, 0
    li	x5, 0
	li	x6, 0
    li	x7, 0
    li	x8, 0
    li	x9, 0
    li	x10, 0
    li	x11, 0
    li	x12, 0
    li	x13, 0
    li	x14, 0
    li	x15, 0
    li	x16, 0
    li	x17, 0
    li	x18, 0
    li	x19, 0
    li	x20, 0
    li	x21, 0
    li	x22, 0
    li	x23, 0
    li	x24, 0
    li	x25, 0
    li	x26, 0
    li	x27, 0
    li	x28, 0
    li	x29, 0
    li	x30, 0
    li	x31, 0

    /* initialize global pointer */
    lui	gp, %hi( _gp)
    addi	gp,	gp, %lo( _gp)

init_bss:
    /* init bss section */
    lui	a0, %hi( __bss_start)
    addi	a0,	a0, %lo( __bss_start)
    lui	a1, %hi( __bss_end)
    addi	a1,	a1, %lo( __bss_end)
    li	a2, 0x0
    jal	fill_block

init_sbss:
    /* init bss section */
    lui	a0, %hi( __sbss_start)
    addi	a0,	a0, %lo( __sbss_start)
    lui	a1, %hi( __sbss_end)
    addi	a1,	a1, %lo( __sbss_end)
    li	a2, 0x0
    jal	fill_block

init_sbss2:
    /* init bss section */
    lui	a0, %hi( __sbss2_start)
    addi	a0,	a0, %lo( __sbss2_start)
    lui	a1, %hi( __sbss2_end)
    addi	a1,	a1, %lo( __sbss2_end)
    li	a2, 0x0
    jal	fill_block

write_stack_pattern:
    /* init bss section */
    lui	a0, %hi( _stack_end  /* note the stack grows from top to bottom */)
    addi	a0,	a0, %lo( _stack_end  /* note the stack grows from top to bottom */)
    lui	a1, %hi( __stack)
    addi	a1,	a1, %lo( __stack)
    li	a2, 0xABABABAB
    jal	fill_block

init_stack:
    /* set stack pointer */
    lui	sp, %hi( _stack)
    addi	sp,	sp, %lo( _stack)
    /* start program, return to error handler */
	lui	ra, %hi( prvTaskExitError)
    addi	ra,	ra, %lo( prvTaskExitError)
	j	main

/* When trap is an interrupt, this function is called */
interrupt:
	srli	t0,t0,1
	beq		t0,x0,softwareInterrupt
	LOAD	t0, 0x0(sp)
	addi	sp, sp, REGBYTES
	/* Interupt is timer interrupt, return via mret */
	j		vPortInterruptHandler

softwareInterrupt:
	/* Interupt is software interrupt */
	mret


/* For when a trap is fired */
trap_entry:
	/* Check for interrupt */
	addi	sp, sp, -REGBYTES
	STORE	t0, 0x0(sp)
	csrr	t0, mcause
	blt		t0,x0,interrupt
	LOAD	t0, 0x0(sp)
	addi	sp, sp, REGBYTES

	/* System call and other traps */
	addi sp, sp, -REGBYTES*31
	STORE x1, 1*REGBYTES(sp)
	STORE x2, 2*REGBYTES(sp)
	STORE x3, 3*REGBYTES(sp)
	STORE x4, 4*REGBYTES(sp)
	STORE x5, 5*REGBYTES(sp)
	STORE x6, 6*REGBYTES(sp)
	STORE x7, 7*REGBYTES(sp)
	STORE x8, 8*REGBYTES(sp)
	STORE x9, 9*REGBYTES(sp)
	STORE x10, 10*REGBYTES(sp)
	STORE x11, 11*REGBYTES(sp)
	STORE x12, 12*REGBYTES(sp)
	STORE x13, 13*REGBYTES(sp)
	STORE x14, 14*REGBYTES(sp)
	STORE x15, 15*REGBYTES(sp)
	STORE x16, 16*REGBYTES(sp)
	STORE x17, 17*REGBYTES(sp)
	STORE x18, 18*REGBYTES(sp)
	STORE x19, 19*REGBYTES(sp)
	STORE x20, 20*REGBYTES(sp)
	STORE x21, 21*REGBYTES(sp)
	STORE x22, 22*REGBYTES(sp)
	STORE x23, 23*REGBYTES(sp)
	STORE x24, 24*REGBYTES(sp)
	STORE x25, 25*REGBYTES(sp)
	STORE x26, 26*REGBYTES(sp)
	STORE x27, 27*REGBYTES(sp)
	STORE x28, 28*REGBYTES(sp)
	STORE x29, 29*REGBYTES(sp)
	STORE x30, 30*REGBYTES(sp)
	STORE x31, 31*REGBYTES(sp)
#        lui      a0, %hi( _port_isr_count)
#    addi      a0,      a0, %lo( _port_isr_count)
#        lw      a1, 0(a0)
#        addi    a1, a1, 1
#        sw      a1, 0(a0)

	csrr a0, mcause
	csrr a1, mepc
	mv a2, sp
	jal ulSyscallTrap
	csrw mepc, a0

#        lui      a0, %hi( _port_isr_count)
#    addi      a0,      a0, %lo( _port_isr_count)
#        lw      a1, 0(a0)
#        addi    a1, a1, 1
#        sw      a1, 0(a0)

	LOAD x1, 1*REGBYTES(sp)
	LOAD x2, 2*REGBYTES(sp)
	LOAD x3, 3*REGBYTES(sp)
	LOAD x4, 4*REGBYTES(sp)
	LOAD x5, 5*REGBYTES(sp)
	LOAD x6, 6*REGBYTES(sp)
	LOAD x7, 7*REGBYTES(sp)
	LOAD x8, 8*REGBYTES(sp)
	LOAD x9, 9*REGBYTES(sp)
	LOAD x10, 10*REGBYTES(sp)
	LOAD x11, 11*REGBYTES(sp)
	LOAD x12, 12*REGBYTES(sp)
	LOAD x13, 13*REGBYTES(sp)
	LOAD x14, 14*REGBYTES(sp)
	LOAD x15, 15*REGBYTES(sp)
	LOAD x16, 16*REGBYTES(sp)
	LOAD x17, 17*REGBYTES(sp)
	LOAD x18, 18*REGBYTES(sp)
	LOAD x19, 19*REGBYTES(sp)
	LOAD x20, 20*REGBYTES(sp)
	LOAD x21, 21*REGBYTES(sp)
	LOAD x22, 22*REGBYTES(sp)
	LOAD x23, 23*REGBYTES(sp)
	LOAD x24, 24*REGBYTES(sp)
	LOAD x25, 25*REGBYTES(sp)
	LOAD x26, 26*REGBYTES(sp)
	LOAD x27, 27*REGBYTES(sp)
	LOAD x28, 28*REGBYTES(sp)
	LOAD x29, 29*REGBYTES(sp)
	LOAD x30, 30*REGBYTES(sp)
	LOAD x31, 31*REGBYTES(sp)

	addi sp, sp, REGBYTES*31
	mret

/* Fills memory blocks */
fill_block:
/*
a0 = start
a1 = end
a2 = value
This code overwrites the last word in the range
*/
    bgeu	a0, a1, fb_end
    sw		a2, 0(a0)
    addi	a0, a0, 4
    j		fill_block
fb_end:
    ret

debug_mstatus:
	csrr	a0, mstatus
	ret

debug_mip:
	csrr	a0, mip
	ret
	
debug_mie:
	csrr	a0, mie
	ret
	
debug_mtvec:
	csrr	a0, mtvec
	ret
	
