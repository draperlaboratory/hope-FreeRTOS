RISCV_XLEN ?= 32
RISCV_LIB  ?= elf
CCPATH =
ARCH 		= -march=rv32im
ABI 		= -mabi=ilp32


TARGET=$(CCPATH)riscv${RISCV_XLEN}-unknown-${RISCV_LIB}

GCC		= $(TARGET)-gcc
OBJCOPY	= $(TARGET)-objcopy
OBJDUMP	= $(TARGET)-objdump
AR		= $(TARGET)-ar
RANLIB	= $(TARGET)-ranlib

# Use main_blinky as demo source and target file name if not specified
PROG 	?= main_blinky
CRT0	= bsp/boot.S

# For debugging
$(info $$PROG is [${PROG}])

FREERTOS_SOURCE_DIR	= ../../Source
FREERTOS_PLUS_SOURCE_DIR = ../../../FreeRTOS-Plus/Source
FREERTOS_TCP_SOURCE_DIR = $(FREERTOS_PLUS_SOURCE_DIR)/FreeRTOS-Plus-TCP

WARNINGS= -Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused


# Root of RISC-V tools installation
RISCV ?= /opt/riscv

FREERTOS_SRC = \
	$(FREERTOS_SOURCE_DIR)/croutine.c \
	$(FREERTOS_SOURCE_DIR)/list.c \
	$(FREERTOS_SOURCE_DIR)/queue.c \
	$(FREERTOS_SOURCE_DIR)/tasks.c \
	$(FREERTOS_SOURCE_DIR)/timers.c \
	$(FREERTOS_SOURCE_DIR)/event_groups.c \
	$(FREERTOS_SOURCE_DIR)/portable/MemMang/heap_4.c # TODO TEST

APP_SOURCE_DIR	= ../Common/Minimal

PORT_SRC = $(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/port.c
PORT_ASM = $(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/portASM.S

INCLUDES = \
	-I. \
	-I./bsp \
	-I$(FREERTOS_SOURCE_DIR)/include \
	-I../Common/include \
	-I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V

ASFLAGS  += -g $(ARCH) $(ABI)  -Wa,-Ilegacy -I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions -DportasmHANDLE_INTERRUPT=external_interrupt_handler

CFLAGS = $(WARNINGS) $(INCLUDES) -DmainDEMO_TYPE=$(DEMO_TYPE)
CFLAGS += -O0 -g3 $(ARCH) $(ABI) -mcmodel=medany

DEMO_SRC = main.c \
	demo/$(PROG).c

APP_SRC = \
	bsp/bsp.c \
	bsp/plic_driver.c \
	bsp/syscalls.c \
	bsp/uart_16550.c \
	bsp/xilinx/uartns550/xuartns550.c \
	bsp/xilinx/uartns550/xuartns550_g.c \
	bsp/xilinx/uartns550/xuartns550_sinit.c \
	bsp/xilinx/uartns550/xuartns550_selftest.c \
	bsp/xilinx/uartns550/xuartns550_stats.c \
	bsp/xilinx/uartns550/xuartns550_options.c \
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
	bsp/xilinx/common/xbasic_types.c \
	bsp/xilinx/common/xil_io.c \
	bsp/xilinx/common/xil_assert.c

INCLUDES = \
	-I. \
	-I./bsp \
	-I./bsp/xilinx \
	-I./bsp/xilinx/common \
	-I./bsp/xilinx/axidma \
	-I./bsp/xilinx/axiethernet \
	-I./bsp/xilinx/uartns550 \
	-I$(FREERTOS_SOURCE_DIR)/include \
	-I../Common/include \
	-I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V

ASFLAGS  += -g $(ARCH) $(ABI)  -Wa,-Ilegacy -I$(FREERTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions -DportasmHANDLE_INTERRUPT=external_interrupt_handler

CFLAGS = $(WARNINGS) $(INCLUDES)
CFLAGS += -O0 -g3 $(ARCH) $(ABI) -mcmodel=medany

ifeq ($(PROG),main_blinky)
	CFLAGS += -DmainDEMO_TYPE=1
else 
ifeq ($(PROG),main_full)
	CFLAGS += -DmainDEMO_TYPE=2
	PORT_ASM += demo/RegTest.S
	FREERTOS_SRC += $(FREERTOS_SOURCE_DIR)/stream_buffer.c
	APP_SRC +=  \
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
ifeq ($(PROG),main_drivers)
	CFLAGS += -DmainDEMO_TYPE=3
	DEMO_SRC += demo/xaxiethernet_example_util.c
	INCLUDES += -I./demo
else
ifeq ($(PROG),main_ping)
	CFLAGS += -DmainDEMO_TYPE=4
	DEMO_SRC += demo/xaxiethernet_example_util.c
	INCLUDES += -I./demo
else
ifeq ($(PROG),main_tcp)
	CFLAGS += -DmainDEMO_TYPE=5
	INCLUDES += \
		-I./demo \
		-I$(FREERTOS_TCP_SOURCE_DIR) \
		-I$(FREERTOS_TCP_SOURCE_DIR)/include \
		-I$(FREERTOS_TCP_SOURCE_DIR)/portable/Compiler/GCC
	FREERTOS_SRC += \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_IP.c \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_ARP.c \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_DHCP.c \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_DNS.c \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_Sockets.c \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_TCP_IP.c \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_UDP_IP.c \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_TCP_WIN.c \
		$(FREERTOS_TCP_SOURCE_DIR)/FreeRTOS_Stream_Buffer.c \
		$(FREERTOS_TCP_SOURCE_DIR)/portable/BufferManagement/BufferAllocation_1.c \
		$(FREERTOS_TCP_SOURCE_DIR)/portable/NetworkInterface/RISC-V/NetworkInterface.c
	APP_SRC += \
		bsp/rand.c
	DEMO_SRC += \
		demo/SimpleUDPClientAndServer.c

else
	$(info unknown demo: $(PROG))
endif
endif
endif
endif
endif

ARFLAGS=crsv

#
# Define all object files.
#
RTOS_OBJ = $(FREERTOS_SRC:.c=.o)
APP_OBJ  = $(APP_SRC:.c=.o)
PORT_OBJ = $(PORT_SRC:.c=.o)
DEMO_OBJ = $(DEMO_SRC:.c=.o)
PORT_ASM_OBJ = $(PORT_ASM:.S=.o)
CRT0_OBJ = $(CRT0:.S=.o)
OBJS = $(CRT0_OBJ) $(PORT_ASM_OBJ) $(PORT_OBJ) $(RTOS_OBJ) $(DEMO_OBJ) $(APP_OBJ)

LDFLAGS	 = -T link.ld -nostartfiles -nostdlib -defsym=_STACK_SIZE=4K
LIBS	 =  -lc -lgcc

$(info ASFLAGS=$(ASFLAGS))
$(info LDLIBS=$(LDLIBS))
$(info CFLAGS=$(CFLAGS))
$(info LDFLAGS=$(LDFLAGS))
$(info ARFLAGS=$(ARFLAGS))

%.o: %.c
	@echo "    CC $<"
	@$(GCC) -c $(CFLAGS) -o $@ $<

%.o: %.S
	@echo "    CC $<"
	@$(GCC) $(ASFLAGS) -c $(CFLAGS) -o $@ $<

all: $(PROG).elf

$(PROG).elf  : $(OBJS) Makefile 
	@echo Linking....
	@$(GCC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
	@$(OBJDUMP) -S $(PROG).elf > $(PROG).asm	
	@echo Completed $@

clean :
	@rm -f $(OBJS)
	@rm -f $(PROG).elf 
	@rm -f $(PROG).map
	@rm -f $(PROG).asm

docs :
	@doxygen
