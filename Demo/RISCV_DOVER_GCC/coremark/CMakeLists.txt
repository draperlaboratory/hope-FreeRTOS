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

project (coremark C ASM)

include(${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/CMake_include.txt)
add_executable(coremark ${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/coremark/core_list_join.c  ${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/coremark/core_main.c  ${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/coremark/core_matrix.c  ${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/coremark/core_state.c  ${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/coremark/core_util.c ${FREE_RTOS_DIR}/Demo/RISCV_DOVER_GCC/coremark/core_portme.c)

#set(FLAGS_STR "-O2 -fno-common -funroll-loops -finline-functions --param max-inline-insns-auto=20 -falign-functions=4 -falign-jumps=4 -falign-loops=4")
set(CMAKE_C_FLAGS "${FLAGS_STR} -DFLAGS_STR='-O0' -DITERATIONS=10000 -DPERFORMANCE_RUN=1")

add_compile_definitions(OPENCV_VERSION=${OpenCV_VERSION})

target_link_libraries(coremark free-rtos free-rtos-dover)
