
BUILD_DIR = ./build
CROSS_COMPILE_PREFIX = riscv32-unknown-elf

LINKER_SCRIPT = lscript.ld
#-----------------------------------------------------------
GCC     = $(CROSS_COMPILE_PREFIX)-gcc
OBJCOPY = $(CROSS_COMPILE_PREFIX)-objcopy
OBJDUMP = $(CROSS_COMPILE_PREFIX)-objdump
AR      = $(CROSS_COMPILE_PREFIX)-ar
RANLIB  = $(CROSS_COMPILE_PREFIX)-ranlib

OUT_ELF = FreeRTOS-web-server.elf
CRT0    := UNKNOWN

CFLAGS  = -Wall -O0 -g3 -fmessage-length=0 -MT"$@" -fomit-frame-pointer -fno-strict-aliasing -fno-builtin -D__gracefulExit -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"
LDFLAGS :=  -nostartfiles -static -Wl,-build-id=none
LDOPTS  := -Wl,-T -Wl,$(LINKER_SCRIPT)
LIB_FLAGS := -Wl,--start-group,-lgcc,-lc,--end-group



# Local Variables:
# mode: makefile
# End:
