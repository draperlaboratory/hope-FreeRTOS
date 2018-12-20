-include BuildEnvironment.mk

BSP_DIR   = ../../../FreeRTOS/Demo/RISCV_HiFive1_GCC/bsp
BSP_INC   = $(BSP_DIR)/include
BSP_LIB   = $(BSP_DIR)/libls 
FREERTOS_DIR = ../../..
FREERTOS_SOURCE_DIR = $(FREERTOS_DIR)/FreeRTOS/Source


FREERTOS_SRC = \
	$(FREERTOS_SOURCE_DIR)/croutine.c \
	$(FREERTOS_SOURCE_DIR)/list.c \
	$(FREERTOS_SOURCE_DIR)/queue.c \
	$(FREERTOS_SOURCE_DIR)/tasks.c \
	$(FREERTOS_SOURCE_DIR)/timers.c \
	$(FREERTOS_SOURCE_DIR)/event_groups.c \
	$(FREERTOS_SOURCE_DIR)/portable/MemMang/heap_4.c
UNUSED_FREERTOS_SRC = \
	$(FREERTOS_SOURCE_DIR)/string.c

FREERTOS_INC = $(FREERTOS_SOURCE_DIR)/include

FREERTOS_INCLUDES := \
	-I $(FREERTOS_INC) \
	-I $(BSP_DIR)/env \
	-I $(BSP_INC)

FREERTOS_BUILD_DIR = $(BUILD_DIR)/FreeRTOS
FREERTOS_OBJS = $(patsubst %.c,$(FREERTOS_BUILD_DIR)/%.o,$(notdir $(FREERTOS_SRC)))
VPATH += $(FREERTOS_SOURCE_DIR) $(FREERTOS_SOURCE_DIR)/portable/MemMang