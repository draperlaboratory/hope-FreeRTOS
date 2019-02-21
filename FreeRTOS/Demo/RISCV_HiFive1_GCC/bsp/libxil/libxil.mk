# See LICENSE for license details.

ifndef _MK_LIBXIL
_MK_LIBXIL := # defined

LIBXIL_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
LIBXIL_DIR := $(LIBXIL_DIR:/=)

LIBXIL_SRCS := \
	libsrc/emacps_v3_2/src/xemacps.c \
	libsrc/emacps_v3_2/src/xemacps_control.c \
	libsrc/emacps_v3_2/src/xemacps_intr.c \
	libsrc/standalone_v5_5/src/xil_assert.c \
	libsrc/standalone_v5_5/src/xil_stubs.c \
	libsrc/standalone_v5_5/src/xil_io.c

INCLUDES = -I$(LIBXIL_DIR)/include
FREERTOS_INCLUDES += $(LIBXIL_DIR)/include

LIBXIL_SRCS := $(foreach f,$(LIBXIL_SRCS),$(LIBXIL_DIR)/$(f))
LIBXIL_OBJS := $(LIBXIL_SRCS:.c=.o)

LIBXIL := libxil.a

LINK_DEPS += $(LIBXIL_DIR)/$(LIBXIL)

LDFLAGS += -L. -Wl,--start-group -lxil -lc -Wl,--end-group

CLEAN_OBJS += $(LIBXIL_OBJS)

$(LIBXIL_OBJS): %.o: %.c $(HEADERS)
	$(GCC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(LIBXIL_DIR)/$(LIBXIL): $(LIBXIL_OBJS)
	$(AR) rcs $@ $^

endif # _MK_LIBXIL
