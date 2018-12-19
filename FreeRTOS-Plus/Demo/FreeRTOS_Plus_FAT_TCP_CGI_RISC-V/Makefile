include BuildEnvironment.mk
include FreeRTOS.mk
include FreeRTOS-Plus.mk
-include $(wildcard $(OUTDIR)/*.d)
# Platform spcific settings for this application _____________________________________________

ARCH = RISCV
ARCH_PORTABLE_INC = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)/
ARCH_PORTABLE_SRC = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)/port.c
ARCH_PORTABLE_ASM = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)/portASM.S

PORT_OBJS := $(patsubst %.c,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(ARCH_PORTABLE_SRC)))
PORT_OBJS += $(patsubst %.S,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(ARCH_PORTABLE_ASM)))
FREERTOS_OBJS += $(PORT_OBJS)
VPATH += $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)

# Set up application source, include, and object files for compilation: ______________________
APP_SRC_DIR = ./src
APP_SRC = \
	$(APP_SRC_DIR)/cgi.c \
	$(APP_SRC_DIR)/cgi_leakage.c \
	$(APP_SRC_DIR)/cgi_overflow.c \
	$(APP_SRC_DIR)/cgi_permissions.c \
	$(APP_SRC_DIR)/hr_gettime.c \
	$(APP_SRC_DIR)/main.c \
	$(APP_SRC_DIR)/memcpy.c \
	$(APP_SRC_DIR)/platform.c \
	$(APP_SRC_DIR)/uncached_memory.c


APP_INCLUDES = \
	-I ./include \
	-I $(FREERTOS_INC) \
	-I $(BSP_INC) \
	-I $(BSP_INC)/devices \
	-I $(BSP_DIR)/env \
	-I $(ARCH_PORTABLE_INC) \
	-I $(FREERTOS_PLUS_DIR)/Demo/Common/Demo_IP_Protocols/include \
	-I $(FREERTOS_PLUS_DIR)/Demo/Common/FreeRTOS_Plus_UDP_Demos/TraceMacros/Example1 \
	-I $(FREERTOS_PLUS_TCP_DIR)/include \
	-I $(FREERTOS_PLUS_TCP_DIR)/protocols/include \
	-I $(FREERTOS_PLUS_TCP_DIR)/portable/Compiler/GCC \
	-I $(FREERTOS_PLUS_TCP_DIR)/portable/NetworkInterface \
	-I $(FREERTOS_PLUS_FAT_DIR)/include \
	-I $(FREERTOS_PLUS_FAT_DIR)/portable/common \
	-I $(FREERTOS_PLUS_CMN_DIR)/Utilities/include \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_CLI_Demos/include \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_TCP_Demos/include \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_TCP_Demos/TraceMacros/Example1
APP_BUILD_DIR = $(BUILD_DIR)/app
APP_OBJS := $(patsubst %.c,$(APP_BUILD_DIR)/%.o,$(notdir $(APP_SRC)))
APP_OBJS += $(patsubst %.S,$(APP_BUILD_DIR)/%.o,$(notdir $(APP_ASM)))
VPATH += $(APP_SRC_DIR)

# Need to tell FreeRTOS where to find the FreeRTOSConfig.h and bsp files ____________________
FREERTOS_INCLUDES += \
	-I ./include \
	-I $(BSP_INC) \
	-I $(ARCH_PORTABLE_INC)

FREERTOS_PLUS_UTIL_INCLUDES += $(FREERTOS_INCLUDES)
FREERTOS_CLI_INCLUDES += $(FREERTOS_INCLUDES)
FREERTOS_TCP_INCLUDES += $(FREERTOS_INCLUDES)
FREERTOS_FAT_INCLUDES += $(FREERTOS_INCLUDES)


# List of object files to compile for the system:
OUT_OBJS = \
	$(APP_OBJS) \
	$(FREERTOS_OBJS) \
	$(FREERTOS_PLUS_UTIL_OBJS) \
	$(FREERTOS_CLI_OBJS) \
	$(FREERTOS_TCP_OBJS) \
	$(FREERTOS_FAT_OBJS)

BUILD_DIRECTORIES = \
	$(APP_BUILD_DIR) \
	$(FREERTOS_BUILD_DIR) \
	$(FREERTOS_PLUS_UTIL_BUILD_DIR) \
	$(FREERTOS_CLI_BUILD_DIR) \
	$(FREERTOS_TCP_BUILD_DIR) \
	$(FREERTOS_FAT_BUILD_DIR)

.PHONY: debug clean app_compile debug-app frtos_compile print-info frtos_p_util frtos_cli frtos_tcp frtos_fat out_elf
all: directories $(OUT_OBJS) $(OUT_ELF)
directories: $(BUILD_DIRECTORIES)
app_compile: directories $(APP_OBJS)
frtos_compile: directories $(FREERTOS_OBJS)
frtos_p_util: directories $(FREERTOS_PLUS_UTIL_OBJS)
frtos_cli: directories $(FREERTOS_CLI_OBJS)
frtos_tcp: directories $(FREERTOS_TCP_OBJS)
frtos_fat: directories $(FREERTOS_FAT_OBJS)
out_elf: directories $(OUT_ELF)

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
	$(GCC) $(CFLAGS) $(APP_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(FREERTOS_BUILD_DIR)/%.o : %.c
	@echo "[FreeRTOS Objects] : $@ __________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(FREERTOS_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(FREERTOS_BUILD_DIR)/%.o : %.S
	@echo "[FreeRTOS Objects] : $@ __________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(FREERTOS_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(FREERTOS_PLUS_UTIL_BUILD_DIR)/%.o: %.c
	@echo "[FreeRTOS Plus Utils] : $@ _______________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(FREERTOS_PLUS_UTIL_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(FREERTOS_CLI_BUILD_DIR)/%.o: %.c
	@echo "[FreeRTOS Plus CLI] : $@ _______________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(FREERTOS_CLI_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(FREERTOS_TCP_BUILD_DIR)/%.o: %.c
	@echo "[FreeRTOS Plus TCP] : $@ _______________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) -std=c11 $(FREERTOS_TCP_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(FREERTOS_FAT_BUILD_DIR)/%.o: %.c
	@echo "[FreeRTOS Plus FAT] : $@ _______________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(FREERTOS_FAT_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

# Generate ELF ___________________________________________________________________________
$(OUT_ELF): $(OUT_OBJS) $(LINKER_SCRIPT) $(XILINX_SPEC) $(BSP_LIB)/libxil.a
	@echo 'Building target: $@'
	@echo '_______________________________'
	$(GCC) $(LDFLAGS) -specs=$(XILINX_SPEC) $(LDOPTS) -L$(BSP_DIR)/lib -o $@ $(OUT_OBJS) $(LIB_FLAGS)
	@echo 'Finished building target: $@'
	@echo ' '

.PHONY:  build_lib
build_lib:
	rm $(BSP_LIB)/libxil.a
	cd $(BSP_DIR) && ./make.sh

$(BSP_LIB)/libxil.a:
	cd $(BSP_DIR) && ./make.sh

$(BUILD_DIRECTORIES):
	mkdir -p $@

clean:
	rm -rv $(BUILD_DIR)

debug:
	@echo "Application Objects: ___________________________"
	@echo $(APP_OBJS)
	@echo "Application Includes: __________________________"
	@echo $(APP_INCLUDES)
	@echo "FreeRTOS Objects: ______________________________"
	@echo $(FREERTOS_OBJS)
	@echo "FREERTOS Includes: _____________________________"
	@echo $(FREERTOS_INCLUDES)
	@echo "OUTPUT Objects: ________________________________"
	@echo $(OUT_OBJS)
	@echo "FREERTOS TCP Objects: __________________________"
	@echo $(FREERTOS_TCP_OBJS)

print-info:
	@echo "VPATH: $(VPATH)"
	@echo "PORT_OBJS: $(PORT_OBJS)"

debug-app:
	@echo "Application Objects: ___________________________"
	@for o in $(APP_OBJS); do \
		echo "  $$o"; \
	done
	@echo "Application Sources: ___________________________"
	@for s in $(APP_SRC); do \
		echo "  $$s"; \
	done
# All of the sources participating in the build are defined here
# -include sources.mk
# -include src/FreeRTOS/portable/MemMang/subdir.mk
# -include src/FreeRTOS/portable/GCC/ARM_CA9/subdir.mk
# -include src/FreeRTOS-Plus-CLI/subdir.mk
# -include src/FreeRTOS+TCP/protocols/HTTP/subdir.mk
# -include src/FreeRTOS+TCP/protocols/FTP/subdir.mk
# -include src/FreeRTOS+TCP/protocols/Common/subdir.mk
# -include src/FreeRTOS+TCP/portable/NetworkInterface/Zynq/subdir.mk
# -include src/FreeRTOS+TCP/portable/BufferManagement/subdir.mk
# -include src/FreeRTOS+TCP/subdir.mk
# -include src/FreeRTOS+FAT/portable/common/subdir.mk
# -include src/FreeRTOS+FAT/portable/Zynq/subdir.mk
# -include src/FreeRTOS+FAT/subdir.mk
# -include src/FreeRTOS/subdir.mk
# -include src/Common_Demo_Utils/Utilities/subdir.mk
# -include src/Common_Demo_Utils/FreeRTOS_Plus_TCP_Demos/TraceMacros/Example1/subdir.mk
# -include src/Common_Demo_Utils/FreeRTOS_Plus_TCP_Demos/subdir.mk
# -include src/Common_Demo_Utils/FreeRTOS_Plus_FAT_Demos/test/subdir.mk
# -include src/Common_Demo_Utils/FreeRTOS_Plus_FAT_Demos/subdir.mk
# -include src/Common_Demo_Utils/FreeRTOS_Plus_CLI_Demos/subdir.mk
# -include src/subdir.mk
# -include subdir.mk
# -include objects.mk

# ifneq ($(MAKECMDGOALS),clean)
# ifneq ($(strip $(S_UPPER_DEPS)),)
# -include $(S_UPPER_DEPS)
# endif
# ifneq ($(strip $(C_DEPS)),)
# -include $(C_DEPS)
# endif
# endif

# -include ../makefile.defs

# # Add inputs and outputs from these tool invocations to the build variables
# ELFSIZE += \
# RTOSDemo.elf.size \


# # All Target
# all: pre-build main-build

# # Main-build Target
# main-build: RTOSDemo.elf secondary-outputs

# # Tool invocations
# RTOSDemo.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
# 	@echo 'Building target: $@'
# 	@echo 'Invoking: ARM v7 gcc linker'
# 	arm-none-eabi-gcc -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -Wl,-build-id=none -specs=Xilinx.spec -Wl,-T -Wl,../src/lscript.ld -L../../RTOSDemo_bsp/ps7_cortexa9_0/lib -o "RTOSDemo.elf" $(OBJS) $(USER_OBJS) $(LIBS)
# 	@echo 'Finished building target: $@'
# 	@echo ' '

# RTOSDemo.elf.size: RTOSDemo.elf
# 	@echo 'Invoking: ARM v7 Print Size'
# 	arm-none-eabi-size RTOSDemo.elf  |tee "RTOSDemo.elf.size"
# 	@echo 'Finished building: $@'
# 	@echo ' '

# # Other Targets
# clean:
# 	-$(RM) $(EXECUTABLES)$(OBJS)$(S_UPPER_DEPS)$(C_DEPS)$(ELFSIZE) RTOSDemo.elf
# 	-@echo ' '

# pre-build:
# 	-a9-linaro-pre-build-step
# 	-@echo ' '

# secondary-outputs: $(ELFSIZE)

# .PHONY: all clean dependents
# .SECONDARY: main-build pre-build

# -include ../makefile.targets
