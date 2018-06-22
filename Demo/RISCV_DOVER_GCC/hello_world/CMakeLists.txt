cmake_minimum_required(VERSION 3.5) # or other version

if (NOT DEFINED FREE_RTOS_DIR)
  set(FREE_RTOS_DIR "$ENV{FREE_RTOS_DIR}")
endif()

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
   set(CMAKE_TOOLCHAIN_FILE "${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/riscv.cmake")
 endif()
 
if (NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

project (hello_world C ASM)

include(${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/CMake_include.txt)
add_executable(hello_world ${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/hello_world/main.c)

target_link_libraries(hello_world free-rtos free-rtos-dover)
