SDK_PATH := FreeRTOS/Demo/RISC-V-Qemu-sifive_e-FreedomStudio/freedom-e-sdk
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
	$(SDK_DRIVER_DIR)/plic/plic_driver.c \
	$(SDK_E300_DIR)/init.c \
	$(SDK_WRAP_DIR)/sys/write.c \
	$(SDK_WRAP_DIR)/sys/isatty.c

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
	$(SDK_DRIVER_DIR)/plic

CFLAGS += -DportasmHANDLE_INTERRUPT=handle_trap -DUSE_PLIC

SDK_CFLAGS := -include sys/cdefs.h

LDFLAGS += -Wl,--wrap=write -Wl,--wrap=isatty
