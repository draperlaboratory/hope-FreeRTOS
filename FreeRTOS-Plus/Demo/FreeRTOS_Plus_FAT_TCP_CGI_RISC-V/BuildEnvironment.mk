
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

COMMON_FLAGS := -MT"$@" -Wall -O0 -g3 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"
COMMON_FLAGS += -msmall-data-limit=8 -ffunction-sections -fdata-sections -fno-builtin-printf

CFLAGS := -std=gnu11
CFLAGS += $(COMMON_FLAGS)

ASMFLAGS :=
ASMFLAGS += $(COMMON_FLAGS)

LDFLAGS := -Xlinker --gc-sections -Xlinker --defsym=__stack_size=1K -O0 -g3
LDFLAGS += -ffunction-sections -fdata-sections --specs=nano.specs -nostartfiles
LDOPTS  := -Wl,-T -Wl,$(LINKER_SCRIPT)



# Local Variables:
# mode: makefile
# End:
