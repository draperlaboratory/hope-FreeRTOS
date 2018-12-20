FREEDOM_SDK_DIR = $(FREERTOS_DIR)/FreeRTOS/Demo/RISC-V-Qemu-sifive_e-FreedomStudio/freedom-e-sdk

FREEDOM_SDK_INCLUDES := \
	-I $(FREEDOM_SDK_DIR)/include \
	-I $(FREEDOM_SDK_DIR)/env/
