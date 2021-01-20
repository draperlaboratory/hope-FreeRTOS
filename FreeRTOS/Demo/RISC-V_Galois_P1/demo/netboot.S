/*
 * This code must remain standalone and position independent, since it will
 * be copied to a scratch area and run from there to avoid writing over
 * itself. Do not reference any globals here; any required values must be
 * available via the arguments provided (and located in memory that isn't about
 * to be clobbered).
 */
	.option norelax

#if __riscv_xlen == 32
#define LOAD lw
#define STORE sw
#else
#define LOAD ld
#define STORE sd
#endif

	/*
	 * Keep start as aligned as the trap vector to ensure copying does not
	 * de-align it.
	 */
	.balign 4
	.global netboot_load_trampoline_start
	.type netboot_load_trampoline_start, @object
netboot_load_trampoline_start:
	/*
	 * a0 - hartid from previous stage
	 * a1 - dtb from previous stage
	 * a2 - dynamic info
	 * a3 - commands
	 * a4 - entry point
	 * a5 - halt before jump
	 */
netboot_load_trampoline:
	/*
	 * We don't want to trap back to the FreeRTOS handler; the handler and/or
	 * any global state used is about to be clobbered. We also don't want to
	 * take any interrupts from now on, so we set mstatus to its reset state
	 * early (must be before we update mtvec).
	 */
	csrw mstatus, zero
	lla t0, netboot_load_trampoline_trap
	csrw mtvec, t0

	/*
	 * Iterate through the null-src-terminated list of load commands.
	  */
1:	LOAD t0, 0*(__riscv_xlen/8)(a3) /* src */
	beqz t0, 8f
	LOAD t1, 1*(__riscv_xlen/8)(a3) /* dst */
	LOAD t2, 2*(__riscv_xlen/8)(a3) /* copysz */
	LOAD t3, 3*(__riscv_xlen/8)(a3) /* zerosz */
	/* Increment early for next iteration */
	addi a3, a3, 4*(__riscv_xlen/8)
	/* t4 is block mask */
	li t4, (__riscv_xlen/8)-1

	/* Copy bytes until src and dst aligned or copysz == 0 */
2:	beqz t2, 2f
	or t5, t0, t1
	and t5, t5, t4
	beqz t5, 3f
	lb t5, 0(t0)
	sb t5, 0(t1)
	addi t0, t0, 1
	addi t1, t1, 1
	addi t2, t2, -1
	j 2b

	/* Copy blocks until copysz <= block mask */
3:	bleu t2, t4, 4f
	LOAD t5, 0(t0)
	STORE t5, 0(t1)
	addi t0, t0, (__riscv_xlen/8)
	addi t1, t1, (__riscv_xlen/8)
	addi t2, t2, -(__riscv_xlen/8)
	j 3b

	/* Copy bytes until copysz == 0 */
4:	beqz t2, 2f
	lb t5, 0(t0)
	sb t5, 0(t1)
	addi t0, t0, 1
	addi t1, t1, 1
	addi t2, t2, -1
	j 4b

	/* Zero bytes until dst aligned or zerosz == 0 */
2:	beqz t3, 2f
	and t5, t1, t4
	beqz t5, 3f
	sb zero, 0(t1)
	addi t1, t1, 1
	addi t3, t3, -1
	j 2b

	/* Zero blocks until zerosz <= block mask */
3:	bleu t3, t4, 4f
	STORE zero, 0(t1)
	addi t1, t1, (__riscv_xlen/8)
	addi t3, t3, -(__riscv_xlen/8)
	j 3b

	/* Zero bytes until zerosz == 0 */
4:	beqz t3, 2f
	sb zero, 0(t1)
	addi t1, t1, 1
	addi t3, t3, -1
	j 4b

	/* Next load command (already incremented after loading this one) */
2:	j 1b

8:	fence rw, rw
	fence.i
	/* Reset mcause */
	csrw mcause, zero
	/* Halt if requested */
	beqz a5, 9f
	ebreak
9:	/* Jump to next stage */
	jr a4
	.size netboot_load_trampoline, . - netboot_load_trampoline

	.balign 4
netboot_load_trampoline_trap:
1:	j 1b /* Spin rather than ebreak to avoid clobbering mcause/mepc */

	.global netboot_load_trampoline_end
	.type netboot_load_trampoline_end, @object
netboot_load_trampoline_end:
	.size netboot_load_trampoline_end, 0
	.size netboot_load_trampoline_start, netboot_load_trampoline_end - netboot_load_trampoline_start
