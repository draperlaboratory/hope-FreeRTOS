
BUILD_DIR = ./build
CROSS_COMPILE_PREFIX = riscv32-unknown-elf

LINKER_SCRIPT = lscript.ld
#-----------------------------------------------------------
GCC     = $(ISP_PREFIX)/bin/clang
OBJCOPY = $(CROSS_COMPILE_PREFIX)-objcopy
OBJDUMP = $(CROSS_COMPILE_PREFIX)-objdump
AR      = $(CROSS_COMPILE_PREFIX)-ar
RANLIB  = $(CROSS_COMPILE_PREFIX)-ranlib
GDB     = $(CROSS_COMPILE_PREFIX)-gdb

OUT_ELF = FreeRTOS-web-server.elf
CRT0    := UNKNOWN

COMMON_FLAGS = -MT"$@" -Wall -O0 -g3 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"
COMMON_FLAGS += -ffunction-sections -fdata-sections -fno-builtin-printf

ARCH_FLAGS = -march=rv32ima -mabi=ilp32 -mcmodel=medium

CFLAGS = -std=gnu11
CFLAGS += -include ${ISP_PREFIX}/include/inline_libs.h
CFLAGS += $(COMMON_FLAGS)
CFLAGS += $(ARCH_FLAGS)
CFLAGS += -DDONT_USE_PLIC -DDONT_USE_M_TIME -Dmalloc\(x\)=pvPortMalloc\(x\) -Dfree\(x\)=vPortFree\(x\)
CFLAGS += -include sys/cdefs.h
CFLAGS += -I ${ISP_PREFIX}/riscv32-unknown-elf/include

ASMFLAGS =
ASMFLAGS += $(COMMON_FLAGS)
ASMFLAGS += $(ARCH_FLAGS)
ASMFLAGS += -DportasmHANDLE_INTERRUPT=handle_trap

LDFLAGS := -Xlinker --defsym=__stack_size=1K -O0 -g3
LDFLAGS += -ffunction-sections -fdata-sections --specs=nano.specs -nostartfiles
LDFLAGS += -T $(LINKER_SCRIPT)
LDFLAGS += -L../
LDFLAGS += -Wl,--start-group -Wl,--end-group
LDFLAGS += -Wl,--wrap=open -Wl,--wrap=lseek -Wl,--wrap=read -Wl,--wrap=write
LDFLAGS += -Wl,--wrap=fstat -Wl,--wrap=stat -Wl,--wrap=close -Wl,--wrap=link -Wl,--wrap=unlink -Wl,--wrap=execve
LDFLAGS += -Wl,--wrap=fork -Wl,--wrap=getpid -Wl,--wrap=kill -Wl,--wrap=wait -Wl,--wrap=isatty -Wl,--wrap=times
LDFLAGS += -Wl,--wrap=sbrk -Wl,--wrap=_exit -Wl,--wrap=puts -Wl,--wrap=_open
LDFLAGS += -Wl,--wrap=_lseek -Wl,--wrap=_read -Wl,--wrap=_write -Wl,--wrap=_fstat -Wl,--wrap=_stat -Wl,--wrap=_close
LDFLAGS += -Wl,--wrap=_link -Wl,--wrap=_unlink -Wl,--wrap=_execve -Wl,--wrap=_fork -Wl,--wrap=_getpid -Wl,--wrap=_kill
LDFLAGS += -Wl,--wrap=_wait -Wl,--wrap=_isatty -Wl,--wrap=_times -Wl,--wrap=_sbrk -Wl,--wrap=__exit -Wl,--wrap=_puts



# Local Variables:
# mode: makefile
# End:
