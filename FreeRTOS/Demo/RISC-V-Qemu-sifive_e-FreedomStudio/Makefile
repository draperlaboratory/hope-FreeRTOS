#
# File: Makefile
# Copyright (c) 2019, Dornerworks Ltd.
#

PROJ_NAME ?= simple
ARCH ?= rv32
ARCH_XLEN = $(subst rv,,$(ARCH))

BUILD_DIR = ./build/$(ARCH)
OUT_LIB = $(BUILD_DIR)/libfreertos-hifive$(ARCH_XLEN).a

include BuildEnvironment.mk
include FreeRTOS.mk
-include $(wildcard $(OUTDIR)/*.d)
# Platform spcific settings for this application _____________________________________________

CFLAGS := $(ISP_CFLAGS)
CFLAGS += $(CONFIG_CFLAGS)
ASMFLAGS := $(ISP_ASMFLAGS)
ASMFLAGS += $(CONFIG_ASMFLAGS)
LDFLAGS := $(ISP_LDFLAGS)

ifneq ($(filter rv64 rv32,$(ARCH)),"")
   BASE_ARCH = RISC-V
endif
ARCH_PORTABLE_INC = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(BASE_ARCH)/
ARCH_PORTABLE_SRC = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(BASE_ARCH)/port.c
ARCH_PORTABLE_ASM = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(BASE_ARCH)/portASM.S

SDK_DIR = ./freedom-e-sdk

LDFLAGS += -L$(BUILD_DIR)
LDFLAGS += -Wl,-Map,"$(BUILD_DIR)/$(PROJ_NAME).map"

SDK_SRC := \
	$(SDK_DIR)/libwrap/sys/_exit.c \
	$(SDK_DIR)/libwrap/sys/close.c \
	$(SDK_DIR)/libwrap/sys/execve.c \
	$(SDK_DIR)/libwrap/sys/fork.c \
	$(SDK_DIR)/libwrap/sys/fstat.c \
	$(SDK_DIR)/libwrap/sys/getpid.c \
	$(SDK_DIR)/libwrap/sys/isatty.c \
	$(SDK_DIR)/libwrap/sys/kill.c \
	$(SDK_DIR)/libwrap/sys/link.c \
	$(SDK_DIR)/libwrap/sys/lseek.c \
	$(SDK_DIR)/libwrap/sys/openat.c \
	$(SDK_DIR)/libwrap/sys/puts.c \
	$(SDK_DIR)/libwrap/sys/printf.c \
	$(SDK_DIR)/libwrap/sys/read.c \
	$(SDK_DIR)/libwrap/sys/sbrk.c \
	$(SDK_DIR)/libwrap/sys/stat.c \
	$(SDK_DIR)/libwrap/sys/times.c \
	$(SDK_DIR)/libwrap/sys/unlink.c \
	$(SDK_DIR)/libwrap/sys/wait.c \
	$(SDK_DIR)/libwrap/sys/write.c \
	$(SDK_DIR)/libwrap/misc/write_hex.c \
	$(SDK_DIR)/env/freedom-e300-hifive1/init.c \
	$(SDK_DIR)/drivers/plic/plic_driver.c \
	$(SDK_DIR)/drivers/fe300prci/fe300prci_driver.c

SDK_ASM := \
	$(SDK_DIR)/env/entry.S \
	$(SDK_DIR)/env/start.S
SDK_OBJS := $(patsubst %.c,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(SDK_SRC)))
SDK_OBJS += $(patsubst %.S,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(SDK_ASM)))
VPATH += \
	$(SDK_DIR)/env \
	$(SDK_DIR)/env/freedom-e300-hifive1 \
	$(SDK_DIR)/libwrap/sys \
	$(SDK_DIR)/libwrap/misc \
	$(SDK_DIR)/libwrap/stdlib \
	$(SDK_DIR)/drivers/plic \
	$(SDK_DIR)/drivers/fe300prci

PORT_OBJS := $(patsubst %.c,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(ARCH_PORTABLE_SRC)))
PORT_OBJS += $(patsubst %.S,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(ARCH_PORTABLE_ASM)))
FREERTOS_OBJS += $(PORT_OBJS) $(SDK_OBJS)
VPATH += $(FREERTOS_SOURCE_DIR)/portable/GCC/$(BASE_ARCH)


CFLAGS += -DportasmHANDLE_INTERRUPT=$(INTERRUPT_HANDLER)

# Set up application source, include, and object files for compilation: ______________________
APP_SRC_DIR = .
APP_SRC = \
	$(APP_SRC_DIR)/main.c \
	$(APP_SRC_DIR)/blinky_demo/main_blinky.c \
	$(APP_SRC_DIR)/full_demo/main_full.c

APP_ASM = \
	$(APP_SRC_DIR)/full_demo/RegTest.S

APP_INCLUDES = \
	-I ./ \
	-I $(SDK_DIR)/include \
	-I $(SDK_DIR)/env/freedom-e300-hifive1 \
	-I $(SDK_DIR)/env \
	-I $(FREERTOS_INC) \
	-I $(ARCH_PORTABLE_INC) \
	-I $(FREERTOS_DIR)/Demo/Common/include

APP_BUILD_DIR = $(BUILD_DIR)/app
APP_OBJS := $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(APP_SRC)))
APP_OBJS += $(patsubst %.S,$(APP_BUILD_DIR)/%.o,$(notdir $(APP_ASM)))
VPATH += \
	$(APP_SRC_DIR)/blinky_demo \
	$(APP_SRC_DIR)/full_demo \
	$(APP_SRC_DIR)/isp \
	$(APP_SRC_DIR)

DEMO_COMMON_SRC =  \
	$(FREERTOS_DIR)/Demo/Common/Minimal/EventGroupsDemo.c \
	$(FREERTOS_DIR)/Demo/Common/Minimal/TimerDemo.c \
	$(FREERTOS_DIR)/Demo/Common/Minimal/TaskNotify.c \
	$(FREERTOS_DIR)/Demo/Common/Minimal/GenQTest.c \
	$(FREERTOS_DIR)/Demo/Common/Minimal/blocktim.c \
	$(FREERTOS_DIR)/Demo/Common/Minimal/dynamic.c \
	$(FREERTOS_DIR)/Demo/Common/Minimal/recmutex.c

DEMO_COMMON_OBJS := $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(DEMO_COMMON_SRC)))
VPATH += $(FREERTOS_DIR)/Demo/Common/Minimal

TEST_SRC_DIR = ./isp
TEST_SRC = \
	$(APP_SRC_DIR)/isp/test_stub.c

TEST_INCLUDES = \
	-I ./isp

TEST_BUILD_DIR = $(BUILD_DIR)/isp
TEST_OBJS := $(patsubst %.c,$(TEST_BUILD_DIR)/%.o,$(notdir $(TEST_SRC)))
VPATH += \
	$(APP_SRC_DIR)/isp

OUT_ELF = $(BUILD_DIR)/FreeRTOS-$(PROJ_NAME).elf

# Need to tell FreeRTOS where to find the FreeRTOSConfig.h __________________________________
FREERTOS_INCLUDES += \
	-I $(ARCH_PORTABLE_INC) \
	-I $(SDK_DIR)/include \
	-I $(SDK_DIR)/env/freedom-e300-hifive1 \
	-I $(SDK_DIR)/env \
	-I $(SDK_DIR)/drivers \
	-I $(FREERTOS_DIR)/Demo/Common/include \
	-I $(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions

# List of object files to compile for the system:
OUT_OBJS = \
	$(APP_OBJS) \
	$(DEMO_COMMON_OBJS) \
	$(FREERTOS_OBJS)

# List of object files to compile for the system:
LIB_OBJS = \
	$(DEMO_COMMON_OBJS) \
	$(FREERTOS_OBJS) \
	$(APP_BUILD_DIR)/lib.o

BUILD_DIRECTORIES = \
	$(APP_BUILD_DIR) \
	$(TEST_BUILD_DIR) \
	$(FREERTOS_BUILD_DIR)

.PHONY: debug clean app_compile debug-app frtos_compile print-info out_elf sim-qemu gdb lib clean-libfreertos-objs
all: directories $(OUT_OBJS) $(OUT_ELF) $(OUT_LIB) $(BUILD_DIR)/libfreertos%.a
directories: $(BUILD_DIRECTORIES)
app_compile: directories $(APP_OBJS)
frtos_compile: directories $(FREERTOS_OBJS)
out_elf: directories $(OUT_ELF)
lib: directories $(OUT_LIB)

# Notes
# Each "Module" needs
#  - Lists of source files
#  - Source Directory(ies)
#  - Include Directory List (with -I prepending each entry)
#  - Build Directory
#  - Object list
#    - OBJS = $(patsubst %.c,$(MODULE_BUILD_DIR)/%.o,$(notdir $(MODULE_SRC)))
#  - Add source direcories to VPATH
# In the master make file
#  - Module build directory must be created
#  - Object Rules can be done with
#    - $(MODULE_BUILD_DIR)/%.o : %.c

# Compile Object Files ____________________________________________________________________
$(APP_BUILD_DIR)/%.o : %.c
	@echo "[APP Objects] : $@ ______________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(APP_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(APP_BUILD_DIR)/%.o : %.S
	@echo "[APP Objects] : $@ ______________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(ASMFLAGS) $(APP_INCLUDES) $(FREERTOS_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(FREERTOS_BUILD_DIR)/%.o : %.c
	@echo "[FreeRTOS Objects] : $@ __________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(FREERTOS_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(FREERTOS_BUILD_DIR)/%.o : %.S
	@echo "[FreeRTOS Objects] : $@ __________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(ASMFLAGS) $(FREERTOS_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(TEST_BUILD_DIR)/%.o : %.c
	@echo "[Test Objects] : $@ ______________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(TEST_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

# Generate ELF ___________________________________________________________________________
$(OUT_ELF): $(OUT_OBJS) $(TEST_OBJS)
	@echo '___________________________________________________________________________________'
	@echo 'Building target: $@'
	@echo '_______________________________'
	$(GCC) $(LDFLAGS) $(LDOPTS) -o $@ $(OUT_OBJS) $(TEST_OBJS) $(LIB_FLAGS)
	@echo 'Finished building target: $@'
	@echo ' '

# Generate lib___________________________________________________________________________
$(OUT_LIB): directories $(LIB_OBJS)
	@echo '___________________________________________________________________________________'
	@echo 'Building target: $@'
	@echo '_______________________________'
	$(AR) rcs $@ $(LIB_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

$(BUILD_DIR)/libfreertos%.a: directories $(LIB_OBJS)
	@echo '___________________________________________________________________________________'
	@echo 'Building libfreertos variant target: $@'
	@echo '_______________________________'
	$(AR) rcs $@ $(LIB_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

.PHONY:  build_lib

$(BUILD_DIRECTORIES):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

clean-libfreertos-objs:
	@echo "Cleaning libfreertos objects"
	rm -rfv $(LIB_OBJS)

sim-qemu: $(OUT_ELF)
	qemu-system-riscv$(ARCH_XLEN) -kernel $^  -S -s -machine sifive_e -nographic

gdb: $(OUT_ELF)
	$(GDB) -iex "set mem inaccessible-by-default off" -iex "set arch riscv:$(ARCH)" -iex "set riscv use_compressed_breakpoint off"
