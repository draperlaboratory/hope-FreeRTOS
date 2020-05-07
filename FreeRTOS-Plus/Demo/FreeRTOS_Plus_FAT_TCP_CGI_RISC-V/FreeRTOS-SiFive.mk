SDK_PATH := FreeRTOS/Demo/RISC-V-ISP/freedom-e-sdk
SDK_DIR := $(FREERTOS_DIR)/$(SDK_PATH)

SDK_WRAP_DIR := $(SDK_DIR)/libwrap
SDK_ENV_DIR  := $(SDK_DIR)/env
SDK_E300_DIR := $(SDK_ENV_DIR)/freedom-e300-hifive1/

SDK_DRIVER_DIR := $(SDK_DIR)/drivers/

SDK_INCLUDES := \
	-I $(SDK_DIR)/include \
	-I $(SDK_ENV_DIR) \
	-I $(SDK_E300_DIR) \
	-I $(SDK_DRIVER_DIR)

SDK_SOURCE := \
	$(SDK_WRAP_DIR)/sys/_exit.c \
	$(SDK_WRAP_DIR)/sys/close.c \
	$(SDK_WRAP_DIR)/sys/execve.c \
	$(SDK_WRAP_DIR)/sys/fork.c \
	$(SDK_WRAP_DIR)/sys/fstat.c \
	$(SDK_WRAP_DIR)/sys/getpid.c \
	$(SDK_WRAP_DIR)/sys/isatty.c \
	$(SDK_WRAP_DIR)/sys/kill.c \
	$(SDK_WRAP_DIR)/sys/link.c \
	$(SDK_WRAP_DIR)/sys/lseek.c \
	$(SDK_WRAP_DIR)/sys/openat.c \
	$(SDK_WRAP_DIR)/sys/puts.c \
	$(SDK_WRAP_DIR)/sys/read.c \
	$(SDK_WRAP_DIR)/sys/sbrk.c \
	$(SDK_WRAP_DIR)/sys/stat.c \
	$(SDK_WRAP_DIR)/sys/times.c \
	$(SDK_WRAP_DIR)/sys/unlink.c \
	$(SDK_WRAP_DIR)/sys/wait.c \
	$(SDK_WRAP_DIR)/sys/write.c \
	$(SDK_WRAP_DIR)/misc/write_hex.c \
	$(SDK_DRIVER_DIR)/plic/plic_driver.c \
	$(SDK_E300_DIR)/init.c \
	$(SDK_DRIVER_DIR)/fe300prci/fe300prci_driver.c

SDK_ASM := \
	$(SDK_ENV_DIR)/start.S \
	$(SDK_ENV_DIR)/entry.S

SDK_BUILD_DIR := $(BUILD_DIR)/SDK
SDK_OBJS := $(patsubst %.c,$(SDK_BUILD_DIR)/%.o,$(notdir $(SDK_SOURCE)))
SDK_OBJS += $(patsubst %.S,$(SDK_BUILD_DIR)/%.o,$(notdir $(SDK_ASM)))

VPATH += \
	$(SDK_ENV_DIR) \
	$(SDK_WRAP_DIR)/sys/ \
	$(SDK_E300_DIR) \
	$(SDK_WRAP_DIR)/misc \
	$(SDK_WRAP_DIR)/stdlib \
	$(SDK_DRIVER_DIR)/plic \
	$(SDK_DRIVER_DIR)/fe300prci

ARCH_FLAGS := -march=rv32ima -mabi=ilp32 -mcmodel=medium --target=riscv32-unknown-elf
CFLAGS     += $(ARCH_FLAGS)
ASMFLAGS   += $(ARCH_FLAGS)
LDFLAGS    += $(ARCH_FLAGS)

CFLAGS += -DUSE_PLIC

ASMFLAGS += -DportasmHANDLE_INTERRUPT=handle_trap

SDK_CFLAGS := -include sys/cdefs.h

LDFLAGS += -Wl,--wrap=free -Wl,--wrap=open -Wl,--wrap=lseek
LDFLAGS += -Wl,--wrap=fstat -Wl,--wrap=stat -Wl,--wrap=close -Wl,--wrap=link
LDFLAGS += -Wl,--wrap=fork -Wl,--wrap=getpid -Wl,--wrap=kill -Wl,--wrap=wait
LDFLAGS += -Wl,--wrap=sbrk -Wl,--wrap=_exit -Wl,--wrap=puts -Wl,--wrap=_malloc
LDFLAGS += -Wl,--wrap=_lseek -Wl,--wrap=_read -Wl,--wrap=_write -Wl,--wrap=_fstat
LDFLAGS += -Wl,--wrap=_link -Wl,--wrap=_unlink -Wl,--wrap=_execve -Wl,--wrap=_fork
LDFLAGS += -Wl,--wrap=_wait -Wl,--wrap=_isatty -Wl,--wrap=_times -Wl,--wrap=_sbrk
LDFLAGS += -Wl,--wrap=write -Wl,--wrap=isatty -Wl,--wrap=read -Wl,--wrap=write
LDFLAGS += -Wl,--wrap=unlink -Wl,--wrap=execve -Wl,--wrap=isatty -Wl,--wrap=times
LDFLAGS += -Wl,--wrap=_free -Wl,--wrap=_open -Wl,--wrap=_stat -Wl,--wrap=_close
LDFLAGS += -Wl,--wrap=_getpid -Wl,--wrap=_kill -Wl,--wrap=__exit -Wl,--wrap=_puts
