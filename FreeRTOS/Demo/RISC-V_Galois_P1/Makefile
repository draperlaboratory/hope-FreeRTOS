# For debugging
$(info $$PROG is [${PROG}])

configCPU_CLOCK_HZ ?=

CCPATH 		?=
TARGET=$(CCPATH)riscv64-unknown-elf

XLEN?=32

ifeq ($(XLEN),64)
	ARCH 		= -march=rv64imac
	ABI 		= -mabi=lp64
	CLANG_ARCH  = riscv64
else
	ARCH 		= -march=rv32im
	ABI 		= -mabi=ilp32
	CLANG_ARCH  = riscv32
endif

# Decide which compiler to use
ifeq ($(USE_CLANG),yes)
	CC		= $(ISP_PREFIX)/bin/clang -arch=$(CLANG_ARCH)
	# CPP		= clang++ --target=$(CLANG_ARCH)
	# TODO: clang has problems with system includes, use g++ for now
	CPP		= $(TARGET)-g++
	# LLVM doesn't support linking RISC-V files yet,
	# so we have to use GCC linker to do so
	LD		= $(TARGET)-gcc
	OBJCOPY	= llvm-objcopy
	# LLVM doesn't uderstand debug symbols created by GCC linker
	# using GCC objdump for the time being (instead of llvm-objdump)
	OBJDUMP	= $(TARGET)-objdump
	AR		= llvm-ar
	RANLIB	= llvm-ranlib
	WERROR = #-Werror
else
	CC		= $(TARGET)-gcc
	CPP		= $(TARGET)-g++
	LD		= $(CC)
	OBJCOPY	= $(TARGET)-objcopy
	OBJDUMP	= $(TARGET)-objdump
	AR		= $(TARGET)-ar
	RANLIB	= $(TARGET)-ranlib
	WERROR = -Werror
endif

$(info CC=${CC})

# Use main_blinky as demo source and target file name if not specified
PROG 	?= main_blinky
LIBFREERTOS = libfreertos-vcu118.a
CRT0	= bsp/boot.S

FREERTOS_SOURCE_DIR	= ../../Source
FREERTOS_PLUS_SOURCE_DIR = ../../../FreeRTOS-Plus/Source
FREERTOS_TCP_SOURCE_DIR = $(FREERTOS_PLUS_SOURCE_DIR)/FreeRTOS-Plus-TCP
FREERTOS_PROTOCOLS_DIR = ./protocols

WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wsign-compare \
		-Waggregate-return -Wmissing-declarations -Wunused $(WERROR) 
 
C_WARNINGS = -Wbad-function-cast -Wmissing-prototypes -Wstrict-prototypes

CPP_WARNINGS = 

CPP_SRC = 

FREERTOS_SRC = \
	$(FREERTOS_SOURCE_DIR)/croutine.c \
	$(FREERTOS_SOURCE_DIR)/list.c \
	$(FREERTOS_SOURCE_DIR)/queue.c \
	$(FREERTOS_SOURCE_DIR)/tasks.c \
	$(FREERTOS_SOURCE_DIR)/timers.c \
	$(FREERTOS_SOURCE_DIR)/event_groups.c \
	$(FREERTOS_SOURCE_DIR)/stream_buffer.c \
	$(FREERTOS_SOURCE_DIR)/portable/MemMang/heap_2.c

APP_SOURCE_DIR	= ../Common/Minimal

PORT_SRC = $(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/port.c
PORT_ASM = $(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/portASM.S

INCLUDES = \
	-I. \
	-I./bsp \
	-I$(FREERTOS_SOURCE_DIR)/include \
	-I../Common/include \
	-I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V

CFLAGS = $(WARNINGS) $(C_WARNINGS) $(INCLUDES) -ffunction-sections -fdata-sections
CFLAGS += -Dmalloc\(x\)=pvPortMalloc\(x\) -Dfree\(x\)=vPortFree\(x\)

ifdef CLANG
CFLAGS += -mcmodel=medium
else
CFLAGS += -mcmodel=medany
endif

DEMO_SRC = main.c \
	demo/$(PROG).c

APP_SRC = \
	bsp/bsp.c \
	bsp/plic_driver.c \
	bsp/syscalls.c \
	bsp/uart.c \
	bsp/iic.c \
	bsp/gpio.c \
	bsp/spi.c \
	bsp/wrap/printf.c \
	bsp/wrap/puts.c \
	bsp/xilinx/uartns550/xuartns550.c \
	bsp/xilinx/uartns550/xuartns550_g.c \
	bsp/xilinx/uartns550/xuartns550_sinit.c \
	bsp/xilinx/uartns550/xuartns550_selftest.c \
	bsp/xilinx/uartns550/xuartns550_stats.c \
	bsp/xilinx/uartns550/xuartns550_options.c \
	bsp/xilinx/uartns550/xuartns550_intr.c \
	bsp/xilinx/uartns550/xuartns550_l.c \
	bsp/xilinx/axidma/xaxidma_bd.c \
	bsp/xilinx/axidma/xaxidma_bdring.c \
	bsp/xilinx/axidma/xaxidma.c \
	bsp/xilinx/axidma/xaxidma_selftest.c \
	bsp/xilinx/axidma/xaxidma_g.c \
	bsp/xilinx/axidma/xaxidma_sinit.c \
	bsp/xilinx/axiethernet/xaxiethernet.c \
	bsp/xilinx/axiethernet/xaxiethernet_control.c \
	bsp/xilinx/axiethernet/xaxiethernet_g.c \
	bsp/xilinx/axiethernet/xaxiethernet_sinit.c \
	bsp/xilinx/iic/xiic.c \
	bsp/xilinx/iic/xiic_g.c \
	bsp/xilinx/iic/xiic_l.c \
	bsp/xilinx/iic/xiic_sinit.c \
	bsp/xilinx/iic/xiic_selftest.c \
	bsp/xilinx/iic/xiic_master.c \
	bsp/xilinx/iic/xiic_intr.c \
	bsp/xilinx/iic/xiic_stats.c \
	bsp/xilinx/spi/xspi.c \
	bsp/xilinx/spi/xspi_g.c \
	bsp/xilinx/spi/xspi_sinit.c \
	bsp/xilinx/spi/xspi_selftest.c \
	bsp/xilinx/spi/xspi_options.c \
	bsp/xilinx/gpio/xgpio.c \
	bsp/xilinx/gpio/xgpio_extra.c \
	bsp/xilinx/gpio/xgpio_g.c \
	bsp/xilinx/gpio/xgpio_intr.c \
	bsp/xilinx/gpio/xgpio_selftest.c \
	bsp/xilinx/gpio/xgpio_sinit.c \
	bsp/xilinx/common/xbasic_types.c \
	bsp/xilinx/common/xil_io.c \
	bsp/xilinx/common/xil_assert.c

INCLUDES = \
	-I. \
	-I./bsp \
	-I./bsp/wrap \
	-I./bsp/xilinx \
	-I./bsp/xilinx/common \
	-I./bsp/xilinx/axidma \
	-I./bsp/xilinx/axiethernet \
	-I./bsp/xilinx/uartns550 \
	-I./bsp/xilinx/iic \
	-I./bsp/xilinx/spi \
	-I./bsp/xilinx/gpio \
	-I$(FREERTOS_SOURCE_DIR)/include \
	-I../Common/include \
	-I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V \
	-I./demo

ASFLAGS  += -g $(ARCH) $(ABI)  -Wa,-Ilegacy \
	-I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions \
	-DportasmHANDLE_INTERRUPT=external_interrupt_handler

FREERTOS_IP_SRC = \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_IP.c \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_ARP.c \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_DHCP.c \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_DNS.c \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_Sockets.c \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_TCP_IP.c \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_UDP_IP.c \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_TCP_WIN.c \
	$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_Stream_Buffer.c \
	$(FREERTOS_TCP_SOURCE_DIR)/portable/BufferManagement/BufferAllocation_2.c \
	$(FREERTOS_TCP_SOURCE_DIR)/portable/NetworkInterface/RISC-V/riscv_hal_eth.c \
	$(FREERTOS_TCP_SOURCE_DIR)/portable/NetworkInterface/RISC-V/NetworkInterface.c \
	bsp/rand.c

FREERTOS_IP_INCLUDE = \
	-I$(FREERTOS_TCP_SOURCE_DIR) \
	-I$(FREERTOS_TCP_SOURCE_DIR)/include \
	-I$(FREERTOS_TCP_SOURCE_DIR)/portable/Compiler/GCC

FREERTOS_IP_DEMO_SRC = \
	demo/SimpleUDPClientAndServer.c \
	demo/TCPEchoClient_SingleTasks.c \
	demo/SimpleTCPEchoServer.c

ifeq ($(PROG),main_blinky)
	CFLAGS += -DmainDEMO_TYPE=1
else 
ifeq ($(PROG),main_full)
	CFLAGS += -DmainDEMO_TYPE=2
	PORT_ASM += demo/RegTest.S
	DEMO_SRC +=  \
		$(APP_SOURCE_DIR)/AbortDelay.c \
		$(APP_SOURCE_DIR)/BlockQ.c \
		$(APP_SOURCE_DIR)/blocktim.c \
		$(APP_SOURCE_DIR)/countsem.c \
		$(APP_SOURCE_DIR)/death.c \
		$(APP_SOURCE_DIR)/dynamic.c \
		$(APP_SOURCE_DIR)/integer.c \
		$(APP_SOURCE_DIR)/MessageBufferDemo.c \
		$(APP_SOURCE_DIR)/PollQ.c \
		$(APP_SOURCE_DIR)/GenQTest.c \
		$(APP_SOURCE_DIR)/QPeek.c \
		$(APP_SOURCE_DIR)/recmutex.c \
		$(APP_SOURCE_DIR)/TimerDemo.c \
		$(APP_SOURCE_DIR)/EventGroupsDemo.c \
		$(APP_SOURCE_DIR)/TaskNotify.c \
		$(APP_SOURCE_DIR)/StreamBufferDemo.c \
		$(APP_SOURCE_DIR)/StreamBufferInterrupt.c \
		$(APP_SOURCE_DIR)/semtest.c
else
ifeq ($(PROG),main_iic)
	CFLAGS += -DmainDEMO_TYPE=3
	INCLUDES += -I./devices
else
ifeq ($(PROG),main_gpio)
	CFLAGS += -DmainDEMO_TYPE=4
	INCLUDES += -I./demo
else
ifeq ($(PROG),main_tcp)
	CFLAGS += -DmainDEMO_TYPE=5
	CFLAGS += -DmainCREATE_TCP_ECHO_SERVER_TASK=1
	INCLUDES += $(FREERTOS_IP_INCLUDE)
	FREERTOS_SRC += $(FREERTOS_IP_SRC)
	DEMO_SRC += $(FREERTOS_IP_DEMO_SRC)

else
ifeq ($(PROG),main_peekpoke)
	CFLAGS += -DmainDEMO_TYPE=9
	CFLAGS += -DmainCREATE_PEEKPOKE_SERVER_TASK=1
	CFLAGS += -DmainCREATE_HTTP_SERVER=1
	CFLAGS += -DipconfigUSE_HTTP=1
	CFLAGS += '-DconfigHTTP_ROOT="/notused"'
	CFLAGS += -DffconfigMAX_FILENAME=4096
	INCLUDES += \
		$(FREERTOS_IP_INCLUDE) \
		-I$(FREERTOS_PROTOCOLS_DIR)/include
	FREERTOS_SRC += \
		$(FREERTOS_IP_SRC) \
		$(FREERTOS_PROTOCOLS_DIR)/Common/FreeRTOS_TCP_server.c \
		$(FREERTOS_PROTOCOLS_DIR)/HTTP/FreeRTOS_HTTP_server.c \
		$(FREERTOS_PROTOCOLS_DIR)/HTTP/FreeRTOS_HTTP_commands.c \
		$(FREERTOS_PROTOCOLS_DIR)/HTTP/peekpoke.c
	DEMO_SRC += $(FREERTOS_IP_DEMO_SRC)
else
ifeq ($(PROG),main_udp)
	CFLAGS += -DmainDEMO_TYPE=5
	CFLAGS += -DmainCREATE_SIMPLE_UDP_CLIENT_SERVER_TASKS=1
	INCLUDES += $(FREERTOS_IP_INCLUDE)
	FREERTOS_SRC += $(FREERTOS_IP_SRC)
	DEMO_SRC += $(FREERTOS_IP_DEMO_SRC)
else
ifeq ($(PROG),main_sd)
	CFLAGS += -DmainDEMO_TYPE=6
	CPPLAGS += -DmainDEMO_TYPE=6
	CPP_SRC += SD/src/SD.cpp \
			   SD/src/File.cpp \
			   SD/src/utility/Sd2Card.cpp \
			   SD/src/utility/SdFile.cpp \
			   SD/src/utility/SdVolume.cpp \
			   SD/src/SDLib.cpp
SD_EXAMPLE ?= CardInfo
USE_RTC_CLOCK ?= 0
$(info SD_EXAMPLE=${SD_EXAMPLE})
ifeq ($(SD_EXAMPLE),ReadWrite)
	CPP_SRC += SD/examples/SdDemo_ReadWrite.cpp
else
ifeq ($(SD_EXAMPLE),DumpFile)
	CPP_SRC += SD/examples/SdDemo_DumpFile.cpp
else
ifeq ($(SD_EXAMPLE),CardInfo)
	CPP_SRC += SD/examples/SdDemo_CardInfo.cpp
else
ifeq ($(SD_EXAMPLE),StressTest)
	DEMO_SRC += SD/examples/SdDemo_StressTest.c
else
$(error unknown SD_EXAMPLE: $(SD_EXAMPLE))
endif
endif
endif
endif

	INCLUDES += -I./SD/src
ifeq ($(USE_RTC_CLOCK),1)
# Below includes for RTC clock (SD FAT time)
	CFLAGS += -DUSE_RTC_CLOCK=1
	CPPLAGS += -DUSE_RTC_CLOCK=1
	INCLUDES += -I./devices
	DEMO_SRC += devices/ds1338rtc.c
endif # USE_RTC_CLOCK
else
ifeq ($(PROG),main_uart)
	CFLAGS += -DmainDEMO_TYPE=7
	INCLUDES += -I./devices
else
ifeq ($(PROG),main_rtc)
RTC_YEAR = $(shell date +%-y)
RTC_MONTH = $(shell date +%-m)
RTC_DAY = $(shell date +%-d)
RTC_HOUR = $(shell date +%-H)
RTC_MINUTE = $(shell date +%-M)
RTC_SECONDS = $(shell date +%-S)
	CFLAGS += -DRTC_YEAR=$(RTC_YEAR)
	CFLAGS += -DRTC_MONTH=$(RTC_MONTH)
	CFLAGS += -DRTC_DAY=$(RTC_DAY)
	CFLAGS += -DRTC_HOUR=$(RTC_HOUR)
	CFLAGS += -DRTC_MINUTE=$(RTC_MINUTE)
	CFLAGS += -DRTC_SECONDS=$(RTC_SECONDS)
	CFLAGS += -DRTC_SET_TIME=1
	CFLAGS += -DmainDEMO_TYPE=10
	INCLUDES += -I./devices
	DEMO_SRC += devices/ds1338rtc.c
else
ifeq ($(PROG),main_uart_malware)
	CFLAGS += -DmainDEMO_TYPE=11
else
ifeq ($(PROG),main_testgen)
	include $(INC_TESTGEN)/testgenEnvironment.mk
else
ifeq ($(PROG),main_netboot)
	CFLAGS += -DmainDEMO_TYPE=0xB002
	PORT_ASM += demo/netboot.S
	INCLUDES += $(FREERTOS_IP_INCLUDE)
	FREERTOS_SRC += $(FREERTOS_IP_SRC)
else
$(error unknown demo: $(PROG))
endif # main_netboot
endif # main_testgen
endif # main_uart_malware
endif # main_rtc
endif # main_uart
endif # main_sd
endif # main_udp
endif # main_peekpoke
endif # main_tcp
endif # main_gpio
endif # main_iic
endif # main_full
endif # main_blinky

ARFLAGS=crsv

ifeq ($(PROG),main_netboot)
	OPT = -O2
else
	OPT = -O0
endif

CFLAGS += $(OPT) -g3 $(ARCH) $(ABI) $(INCLUDES)

# If configCPU_CLOCK_HZ is not empty, pass it as a definition
ifneq ($(configCPU_CLOCK_HZ),)
CFLAGS += -DconfigCPU_CLOCK_HZ=$(configCPU_CLOCK_HZ)
endif
# Disable warnings for C++ for now
CPPFLAGS += $(OPT) -g3 $(ARCH) $(ABI) $(INCLUDES)

# Define all object files.
#
RTOS_OBJ = $(FREERTOS_SRC:.c=.o)
APP_OBJ  = $(APP_SRC:.c=.o)
PORT_OBJ = $(PORT_SRC:.c=.o)
DEMO_OBJ = $(DEMO_SRC:.c=.o)
PORT_ASM_OBJ = $(PORT_ASM:.S=.o)
CRT0_OBJ = $(CRT0:.S=.o)

LIB_OBJS = $(CRT0_OBJ) $(PORT_ASM_OBJ) $(PORT_OBJ) $(RTOS_OBJ) $(APP_OBJ) $(CPP_OBJ)
ELF_OBJS = $(DEMO_OBJ)

LDFLAGS	 = -T link.ld -nostartfiles -nostdlib $(ARCH) $(ABI)
LDFLAGS  += -Wl,--gc-sections
LDFLAGS  += -Wl,--wrap=puts
LDFLAGS  += -Wl,--wrap=printf

LIBS	 =  -lc -lgcc -lfreertos-vcu118 -L.

$(info ASFLAGS=$(ASFLAGS))
$(info LIBS=$(LIBS))
$(info CFLAGS=$(CFLAGS))
$(info LDFLAGS=$(LDFLAGS))
$(info ARFLAGS=$(ARFLAGS))

%.o: %.c
	@echo "    CC $<"
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.cpp
	@echo "    C++ $<"
	$(CPP) -c $(CPPFLAGS) -o $@ $<

%.o: %.S
	@echo "    CC $<"
	$(CC) $(ASFLAGS) -c $(CFLAGS) -o $@ $<

all: $(PROG).elf

lib: $(LIBFREERTOS)

$(PROG).elf  : $(LIBFREERTOS) $(ELF_OBJS) Makefile 
	@echo Linking....
	$(LD) -o $@ $(LDFLAGS) $(ELF_OBJS) $(LIBS)
	@$(OBJDUMP) -S $(PROG).elf > $(PROG).asm	
	@echo Completed $@

$(LIBFREERTOS)  : $(LIB_OBJS) Makefile 
	@echo Linking....
	$(AR) rcs $@ $(LIB_OBJS)
	@echo Completed $@

clean :
	@rm -f $(ELF_OBJS) $(LIB_OBJS) $(LIBFREERTOS)
	@rm -f *.elf *.map *.asm *.o

docs :
	@doxygen
