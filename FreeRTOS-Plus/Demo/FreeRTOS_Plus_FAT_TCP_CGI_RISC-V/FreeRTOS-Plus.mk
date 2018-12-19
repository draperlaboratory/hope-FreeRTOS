FREERTOS_PLUS_DIR = $(FREERTOS_DIR)/FreeRTOS-Plus
FREERTOS_PLUS_TCP_DIR = $(FREERTOS_PLUS_DIR)/Source/FreeRTOS-Plus-TCP
FREERTOS_PLUS_FAT_DIR = $(FREERTOS_PLUS_DIR)/Source/FreeRTOS-Plus-FAT
FREERTOS_PLUS_CMN_DIR = $(FREERTOS_PLUS_DIR)/Demo/Common
FREERTOS_PLUS_IP_DIR = $(FREERTOS_PLUS_CMN_DIR)/Demo_IP_Protocols

# Collect FreeRTOS-Plus Util sources
FREERTOS_PLUS_UTIL_SRC = \
	$(FREERTOS_PLUS_IP_DIR)/Common/UDPLoggingPrintf.c \
	$(FREERTOS_PLUS_IP_DIR)/Common/FreeRTOS_TCP_server.c

FREERTOS_PLUS_UTIL_INCLUDES := \
	-I $(FREERTOS_PLUS_TCP_DIR)/include \
	-I $(FREERTOS_PLUS_TCP_DIR)/portable/Compiler/GCC \
	-I $(FREERTOS_PLUS_TCP_DIR)/protocols/include \
	-I $(FREERTOS_PLUS_IP_DIR)/include \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_UDP_Demos/TraceMacros/Example1 \
	-I $(FREERTOS_PLUS_FAT_DIR)/include
FREERTOS_PLUS_UTIL_BUILD_DIR = $(FREERTOS_BUILD_DIR)/Plus/Utils
FREERTOS_PLUS_UTIL_OBJS = $(patsubst %.c,$(FREERTOS_PLUS_UTIL_BUILD_DIR)/%.o,$(notdir $(FREERTOS_PLUS_UTIL_SRC)))
VPATH += $(FREERTOS_PLUS_IP_DIR)/Common

# Collect FreeRTOS-Plus-TCP sources
FREERTOS_TCP_SRC = \
	$(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_UDP_Demos/TraceMacros/Example1/DemoIPTrace.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_TCP_WIN.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_Stream_Buffer.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_TCP_IP.c \
	$(FREERTOS_PLUS_IP_DIR)/HTTP/FreeRTOS_HTTP_server.c \
	$(FREERTOS_PLUS_IP_DIR)/HTTP/FreeRTOS_HTTP_cgi.c \
	$(FREERTOS_PLUS_IP_DIR)/HTTP/FreeRTOS_HTTP_commands.c \
	$(FREERTOS_PLUS_IP_DIR)/Common/FreeRTOS_TCP_server.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_DHCP.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_Sockets.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_UDP_IP.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_IP.c \
	$(FREERTOS_PLUS_TCP_DIR)/portable/BufferManagement/BufferAllocation_1.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_ARP.c \
	$(FREERTOS_PLUS_TCP_DIR)/FreeRTOS_DNS.c

FREERTOS_TCP_INCLUDES := \
	-I $(FREERTOS_PLUS_IP_DIR)/include \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_UDP_Demos/TraceMacros/Example1 \
	-I $(FREERTOS_PLUS_TCP_DIR)/include \
	-I $(FREERTOS_PLUS_IP_DIR)/include \
	-I $(FREERTOS_PLUS_TCP_DIR)/portable/NetworkInterface \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_UDP_Demos/TraceMacros/Example1 \
	-I $(FREERTOS_PLUS_TCP_DIR)/portable/Compiler/GCC \
	-I $(FREERTOS_PLUS_FAT_DIR)/include
FREERTOS_TCP_BUILD_DIR = $(FREERTOS_BUILD_DIR)/Plus/TCP
FREERTOS_TCP_OBJS = $(patsubst %.c,$(FREERTOS_TCP_BUILD_DIR)/%.o,$(notdir $(FREERTOS_TCP_SRC)))
VPATH += \
	$(FREERTOS_PLUS_TCP_DIR) \
	$(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_UDP_Demos/TraceMacros/Example1 \
	$(FREERTOS_PLUS_IP_DIR) \
	$(FREERTOS_PLUS_IP_DIR)/Common \
	$(FREERTOS_PLUS_IP_DIR)/HTTP \
	$(FREERTOS_PLUS_IP_DIR)/NTP \
	$(FREERTOS_PLUS_TCP_DIR)/portable/BufferManagement \



# Collect FreeRTOS-Plus-FAT sources
FREERTOS_FAT_SRC = \
	$(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_FAT_SL_Demos/CreateExampleFiles/CreateAndVerifyExampleFiles.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_error.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_crc.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_string.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_sys.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_stdio.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_file.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_fat.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_ioman.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_format.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_dir.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_time.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_locking.c \
	$(FREERTOS_PLUS_FAT_DIR)/portable/common/ff_ramdisk.c \
	$(FREERTOS_PLUS_FAT_DIR)/ff_memory.c \

FREERTOS_FAT_INCLUDES := \
	-I $(FREERTOS_PLUS_FAT_DIR)/include \
	-I $(FREERTOS_PLUS_FAT_DIR)/portable/common \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_UDP_Demos/TraceMacros/Example1 \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_FAT_SL_Demos/CreateExampleFiles \
	-I $(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_FAT_SL_Demos/CreateExampleFiles/generated

FREERTOS_FAT_BUILD_DIR = $(FREERTOS_BUILD_DIR)/Plus/FAT
FREERTOS_FAT_OBJS = $(patsubst %.c,$(FREERTOS_FAT_BUILD_DIR)/%.o,$(notdir $(FREERTOS_FAT_SRC)))
VPATH += \
	$(FREERTOS_PLUS_FAT_DIR) \
	$(FREERTOS_PLUS_CMN_DIR)/FreeRTOS_Plus_FAT_SL_Demos/CreateExampleFiles \
	$(FREERTOS_PLUS_FAT_DIR)/portable/Zynq \
	$(FREERTOS_PLUS_FAT_DIR)/portable/common
