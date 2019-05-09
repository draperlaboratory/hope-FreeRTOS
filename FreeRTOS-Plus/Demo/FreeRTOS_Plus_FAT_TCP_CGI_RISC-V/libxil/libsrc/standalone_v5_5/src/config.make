LIBSOURCES = xil_io.c xil_assert.c sleep.c xtime_l.c

# stubs
LIBSOURCES += xil_stubs.c

PROFILE_ARCH_OBJS = profile_mcount_arm.o
LIBS = standalone_libs
