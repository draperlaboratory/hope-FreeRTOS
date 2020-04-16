#
# File: BuildEnvironment.mk
# Copyright (c) 2019, Dornerworks Ltd.
#

#-----------------------------------------------------------
GCC     = $(ISP_PREFIX)/bin/clang

OBJCOPY = $(ISP_PREFIX)/bin/llvm-objcopy
OBJDUMP = $(ISP_PREFIX)/bin/llvm-objdump
AR      = $(ISP_PREFIX)/bin/llvm-ar
RANLIB  = $(ISP_PREFIX)/bin/llvm-ranlib
GDB     = $(ISP_PREFIX)/bin/riscv64-unknown-elf-gdb

INTERRUPT_HANDLER = handle_trap
MSI_HANDLER = as_yet_unhandled

# if using the multi-arch (riscv64-unknown-elf-gcc):
ifneq ($(ARCH), rv64)
ARCH_FLAGS = -march=rv32ima -mabi=ilp32 -mcmodel=medium --target=riscv32-unknown-elf
else
ARCH_FLAGS = -march=rv64imafd -mabi=lp64d -mcmodel=medany --target=riscv64-unknown-elf
endif

# Basic ISP_CFLAGS:
ISP_CFLAGS  = -Wall -Wextra -O0 -g3 -std=gnu11 -mno-relax
ISP_CFLAGS += -ffunction-sections -fdata-sections -fno-builtin-printf
ISP_CFLAGS += -DDONT_USE_PLIC -DDONT_USE_M_TIME -Dmalloc\(x\)=pvPortMalloc\(x\) -Dfree\(x\)=vPortFree\(x\)
ISP_CFLAGS += -include sys/cdefs.h
ISP_CFLAGS += $(ARCH_FLAGS)
ISP_CFLAGS += -I $(ISP_PREFIX)/clang_sysroot/riscv64-unknown-elf/include
# These flags are for outputing *.d dependency files for make

ISP_ASMFLAGS =  -O0 -g3 -mno-relax
ISP_ASMFLAGS += $(ARCH_FLAGS)
ISP_ASMFLAGS += -DportasmHANDLE_INTERRUPT=$(INTERRUPT_HANDLER)
ISP_ASMFLAGS += -DportasmMSI_HANDLER=$(MSI_HANDLER)
ISP_ASMFLAGS += -ffunction-sections -fdata-sections
ISP_ASMFLAGS += -x assembler-with-cpp

LIBWRAP_SYMS := open lseek read write fstat stat close link unlink \
	execve fork getpid kill wait isatty times sbrk _exit puts printf

# Linker arguments __________________________________________
ISP_LDFLAGS := -Xlinker --defsym=__stack_size=1K
ISP_LDFLAGS += -O0 -g3
ISP_LDFLAGS += -ffunction-sections -fdata-sections --specs=nano.specs
ISP_LDFLAGS += -nostartfiles
ISP_LDFLAGS += -T $(LINKER_SCRIPT) -fuse-ld=lld

ISP_LDFLAGS += $(foreach s,$(LIBWRAP_SYMS),-Wl,--wrap=$(s))
ISP_LDFLAGS += $(foreach s,$(LIBWRAP_SYMS),-Wl,--wrap=_$(s))
