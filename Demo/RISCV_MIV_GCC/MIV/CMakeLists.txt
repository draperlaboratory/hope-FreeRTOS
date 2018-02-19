cmake_minimum_required(VERSION 3.5) # or other version

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#   set(CMAKE_TOOLCHAIN_FILE "$ENV{DOVER}/bin/riscv.cmake")
   set(CMAKE_TOOLCHAIN_FILE "../riscv.cmake")
endif()
if (NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

project (FreeRTOS-support C ASM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

if (NOT DEFINED FREE_RTOS_DIR)
  set(FREE_RTOS_DIR "$ENV{DOVER}/FreeRTOS-RISCV")
#  set(FREE_RTOS_DIR "/home/eli/src/FreeRTOS-RISCV")
endif()

include_directories("include")
include_directories("${FREE_RTOS_DIR}/Source/include")
include_directories("${FREE_RTOS_DIR}/Source/portable/GCC/RISCV")
include_directories("${FREE_RTOS_DIR}/Demo/riscv-spike/conf")
#include_directories("${FREE_RTOS_DIR}/Demo/riscv-spike/conf")

#add_library(free-rtos-support src/ns16550.c
add_library(free-rtos-support src/miv_core_uart.c
  src/uart_task.c
  src/sim_control.S)
