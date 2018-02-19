

/* Trap handler */
unsigned long ulSyscallTrap(long cause, long epc, long regs[32])
{
  
//	  panic(cause, epc, regs);
	return epc + 4;
}
