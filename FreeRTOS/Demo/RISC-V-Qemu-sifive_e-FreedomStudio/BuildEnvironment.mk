#
# File: BuildEnvironment.mk
# Copyright (c) 2019, Dornerworks Ltd.
#

BUILD_DIR = ./build
CROSS_COMPILE_PREFIX = riscv32-unknown-elf

SDK_DIR ?= ./freedom-e-sdk

LINKER_SCRIPT = $(SDK_DIR)/env/freedom-e300-hifive1/flash.lds
#-----------------------------------------------------------
GCC     = $(ISP_PREFIX)/bin/clang
OBJCOPY = $(CROSS_COMPILE_PREFIX)-objcopy
OBJDUMP = $(CROSS_COMPILE_PREFIX)-objdump
AR      = $(CROSS_COMPILE_PREFIX)-ar
RANLIB  = $(CROSS_COMPILE_PREFIX)-ranlib
GDB     = $(CROSS_COMPILE_PREFIX)-gdb

# if using the multi-arch (riscv64-unknown-elf-gcc):
ARCH_FLAGS = -march=rv32ima -mabi=ilp32 -mcmodel=medany
# Basic ISP_CFLAGS:
ISP_CFLAGS  = -Wall -Wextra -O0 -g3 -msmall-data-limit=8 -std=gnu11
ISP_CFLAGS += -ffunction-sections -fdata-sections -fno-builtin-printf
ISP_CFLAGS += -DDONT_USE_PLIC -DDONT_USE_M_TIME
ISP_CFLAGS += -include sys/cdefs.h
ISP_CFLAGS += $(ARCH_FLAGS)
# These flags are for outputing *.d dependency files for make
ISP_CFLAGS += -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"

ISP_ASMFLAGS =  -O0 -g3
ISP_ASMFLAGS += $(ARCH_FLAGS)
ISP_ASMFLAGS += -DportasmHANDLE_INTERRUPT=handle_trap
ISP_ASMFLAGS += -msmall-data-limit=8
ISP_ASMFLAGS += -ffunction-sections -fdata-sections
ISP_ASMFLAGS += -x assembler-with-cpp
ISP_ASMFLAGS += -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"

# Linker arguments __________________________________________
ISP_LDFLAGS :=  -Xlinker --gc-sections -Xlinker --defsym=__stack_size=1K
ISP_LDFLAGS += -O0 -g3
ISP_LDFLAGS += -ffunction-sections -fdata-sections --specs=nano.specs
ISP_LDFLAGS += -nostartfiles
ISP_LDFLAGS += -T $(LINKER_SCRIPT)
ISP_LDFLAGS += -L../
ISP_LDFLAGS += -Wl,--start-group -Wl,--end-group
ISP_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=open -Wl,--wrap=lseek -Wl,--wrap=read -Wl,--wrap=write
ISP_LDFLAGS += -Wl,--wrap=fstat -Wl,--wrap=stat -Wl,--wrap=close -Wl,--wrap=link -Wl,--wrap=unlink -Wl,--wrap=execve
ISP_LDFLAGS += -Wl,--wrap=fork -Wl,--wrap=getpid -Wl,--wrap=kill -Wl,--wrap=wait -Wl,--wrap=isatty -Wl,--wrap=times
ISP_LDFLAGS += -Wl,--wrap=sbrk -Wl,--wrap=_exit -Wl,--wrap=puts -Wl,--wrap=_malloc -Wl,--wrap=_free -Wl,--wrap=_open
ISP_LDFLAGS += -Wl,--wrap=_lseek -Wl,--wrap=_read -Wl,--wrap=_write -Wl,--wrap=_fstat -Wl,--wrap=_stat -Wl,--wrap=_close
ISP_LDFLAGS += -Wl,--wrap=_link -Wl,--wrap=_unlink -Wl,--wrap=_execve -Wl,--wrap=_fork -Wl,--wrap=_getpid -Wl,--wrap=_kill
ISP_LDFLAGS += -Wl,--wrap=_wait -Wl,--wrap=_isatty -Wl,--wrap=_times -Wl,--wrap=_sbrk -Wl,--wrap=__exit -Wl,--wrap=_puts
