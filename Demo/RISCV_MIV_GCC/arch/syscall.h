

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

/* Trap handler */
unsigned long ulSyscallTrap(long cause, long epc, long regs[32]);

#endif /* SYSCALL_H */
