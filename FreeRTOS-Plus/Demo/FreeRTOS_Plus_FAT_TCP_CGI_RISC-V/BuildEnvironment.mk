
BUILD_DIR = ./build
CROSS_COMPILE_PREFIX = riscv64-unknown-elf

LINKER_SCRIPT = lscript.ld
#-----------------------------------------------------------
GCC     = $(CROSS_COMPILE_PREFIX)-gcc
OBJCOPY = $(CROSS_COMPILE_PREFIX)-objcopy
OBJDUMP = $(CROSS_COMPILE_PREFIX)-objdump
AR      = $(CROSS_COMPILE_PREFIX)-ar
RANLIB  = $(CROSS_COMPILE_PREFIX)-ranlib

OUT_ELF = FreeRTOS-web-server.elf
CRT0    := UNKNOWN

CFLAGS  = -march=rv32imac -mabi=ilp32 -Wall -O0 -g3 -fmessage-length=0 -MT"$@" -fomit-frame-pointer -fno-strict-aliasing -fno-builtin -D__gracefulExit -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"
LDFLAGS :=  -march=rv32imac -mabi=ilp32 -nostartfiles -static -Wl,-build-id=none
LDOPTS  := -Wl,-T -Wl,$(LINKER_SCRIPT)



# Local Variables:
# mode: makefile
# End:
