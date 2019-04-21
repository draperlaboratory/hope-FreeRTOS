include BuildEnvironment.mk
include FreeRTOS.mk
include FreeRTOS-Plus.mk
include FreeRTOS-SiFive.mk
-include $(wildcard $(OUTDIR)/*.d)
# Platform spcific settings for this application _____________________________________________

ARCH = RISC-V
EXTENSIONS = RV32I_CLINT_no_extensions
ARCH_PORTABLE_INC = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)/
ARCH_PORTABLE_CLINC = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)/CLINT
ARCH_PORTABLE_EXT = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)/chip_specific_extensions/$(EXTENSIONS)
ARCH_PORTABLE_SRC = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)/port.c
ARCH_PORTABLE_ASM = $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)/portASM.S

PORT_OBJS := $(patsubst %.c,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(ARCH_PORTABLE_SRC)))
PORT_OBJS += $(patsubst %.S,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(ARCH_PORTABLE_ASM)))
FREERTOS_OBJS += $(PORT_OBJS)
VPATH += $(FREERTOS_SOURCE_DIR)/portable/GCC/$(ARCH)

GDB_PORT ?= 3333

# Set up application source, include, and object files for compilation: ______________________
APP_SRC_DIR = ./src
CGI_SRC_DIR = $(APP_SRC_DIR)/cgi-app
MED_COMMON_DIR = $(APP_SRC_DIR)/med-common

APP_SRC = \
	$(APP_SRC_DIR)/cgi.c \
	$(APP_SRC_DIR)/cgi-args.c \
	$(APP_SRC_DIR)/cgi-util.c \
	$(APP_SRC_DIR)/sleep.c \
	$(APP_SRC_DIR)/hr_gettime.c \
	$(APP_SRC_DIR)/main.c \
	$(APP_SRC_DIR)/memcpy.c \
	$(APP_SRC_DIR)/platform.c \
	$(APP_SRC_DIR)/uncached_memory.c \
	$(APP_SRC_DIR)/dashboard.c \
	$(APP_SRC_DIR)/login.c \
	$(APP_SRC_DIR)/register.c \
	$(APP_SRC_DIR)/auth.c \
	$(APP_SRC_DIR)/database.c \
	$(APP_SRC_DIR)/hashtable.c \
	$(APP_SRC_DIR)/medical.c \
	$(APP_SRC_DIR)/sha1.c \
	$(APP_SRC_DIR)/negative_tests.c \
	$(APP_SRC_DIR)/user.c

APP_INCLUDES = \
	-I ./include \
	-I ./include/med-common \
	-I $(FREERTOS_INC) \
	-I $(BSP_INC) \
	-I $(BSP_INC)/devices \
	-I $(BSP_DIR)/env \
	$(SDK_INCLUDES) \
	-I $(ARCH_PORTABLE_INC) \
	-I $(ARCH_PORTABLE_EXT) \
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
	-I $(BSP_DIR)/libxil/include \
	$(SDK_INCLUDES) \
	-I $(ARCH_PORTABLE_INC) \
	-I $(ARCH_PORTABLE_CLINC) \
	-I $(ARCH_PORTABLE_EXT)

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
	$(FREERTOS_FAT_OBJS) \
	$(SDK_OBJS)

BUILD_DIRECTORIES = \
	$(APP_BUILD_DIR) \
	$(FREERTOS_BUILD_DIR) \
	$(FREERTOS_PLUS_UTIL_BUILD_DIR) \
	$(FREERTOS_CLI_BUILD_DIR) \
	$(FREERTOS_TCP_BUILD_DIR) \
	$(FREERTOS_FAT_BUILD_DIR) \
	$(SDK_BUILD_DIR)

.PHONY: debug clean app_compile debug-app frtos_compile print-info frtos_p_util frtos_cli frtos_tcp frtos_fat freedom_sdk out_elf clean-libxil sim-qemu sim-qemu-debug sim-gdb
all: directories $(OUT_OBJS) $(OUT_ELF)
directories: $(BUILD_DIRECTORIES)
app_compile: directories $(APP_OBJS)
frtos_compile: directories $(FREERTOS_OBJS)
frtos_p_util: directories $(FREERTOS_PLUS_UTIL_OBJS)
frtos_cli: directories $(FREERTOS_CLI_OBJS)
frtos_tcp: directories $(FREERTOS_TCP_OBJS)
frtos_fat: directories $(FREERTOS_FAT_OBJS)
freedom_sdk: directories $(SDK_OBJS)
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
	$(GCC) $(ASMFLAGS) $(APP_INCLUDES) -o $@ -c $<
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

$(SDK_BUILD_DIR)/%.o: %.c
	@echo "[Freedom SDK] : $@ _____________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(CFLAGS) $(SDK_CFLAGS) $(SDK_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

$(SDK_BUILD_DIR)/%.o : %.S
	@echo "[Freedom SDK] : $@ _____________________________________________________________"
	@echo "Building: $<"
	$(GCC) $(ASMFLAGS) $(SDK_INCLUDES) -o $@ -c $<
	@echo "Finished Building: $<"

include $(BSP_DIR)/libxil/libxil.mk

# Generate ELF ___________________________________________________________________________
$(OUT_ELF): $(OUT_OBJS) $(LINKER_SCRIPT) $(LINK_DEPS)
	@echo 'Building target: $@'
	@echo '_______________________________'
	$(GCC) $(CFLAGS) $(INCLUDES) $(OUT_OBJS) -o $@ $(LDFLAGS) $(LDOPTS) -L$(BSP_DIR)/libxil/
	@echo 'Finished building target: $@'
	@echo ' '

$(BUILD_DIRECTORIES):
	mkdir -p $@

clean:
	rm -rfv $(BUILD_DIR)
	rm -rfv ./isp-output
	rm -fv status.log
	rm -fv uart.log
	rm -fv $(BSP_DIR)/libxil.a
	rm -fv $(LIBXIL_OBJS)

clean-libxil:
	rm -fv $(CLEAN_OBJS)
	rm -fv $(LIBXIL_DIR)/$(LIBXIL)

# Policy Variables ___________________________________________
POLICY_DIR = $(ISP_PREFIX)/kernels
POLICY_RUNTIME ?= hifive
POLICY ?= none
QEMU_GDB = -S -s
QEMU_SERIAL = mon:stdio # Sometimes file:uart.log
ISP_PREFIX ?= /opt/isp
PROJ_NAME = web-server

# Generate ISP Tags _____________________________________________________________________
gen-tags: $(OUT_ELF)
	@echo '___________________________________________________________________________________'
	@echo 'Generating Tags for $^'
	isp_run_app $^ \
	-r hifive \
	--policy none \
	-o ./isp-output \
	--tag-only

sim-qemu: $(OUT_ELF)
	sudo env PATH=$(PATH) ISP_PREFIX=$(ISP_PREFIX) isp_run_app $(OUT_ELF) -p $(POLICY) -s qemu -r frtos \
		-e "-net nic -net tap,ifname=tap0,script=no,downscript=no"

sim-qemu-debug: $(OUT_ELF)
	sudo env PATH=$(PATH) ISP_PREFIX=$(ISP_PREFIX) isp_run_app $(OUT_ELF) -p $(POLICY) -s qemu -r frtos \
		-e "-net nic -net tap,ifname=tap0,script=no,downscript=no" -g $(GDB_PORT)

sim-gdb: $(OUT_ELF)
	isp_debug $(OUT_ELF) $(GDB_PORT)

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
	@echo "Freedom SDK Objects: __________________________"
	@echo $(SDK_OBJS)

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
