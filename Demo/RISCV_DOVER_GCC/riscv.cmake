set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# this is needed to get cmake to do such things as use .o as the default extension
# otherwise you get .obj
set(UNIX 1 CACHE STRING "" FORCE)

# where is the target environment 
if (DEFINED ENV{ISP_PREFIX})
  set(ISP_PREFIX $ENV{ISP_PREFIX})
 else()
   set(ISP_PREFIX "/opt/isp/")
   message (WARNING "You should have defined ISP_PREFIX var !!!")
   message (WARNING "The CMake forced it to point to /opt/isp which might not be whant you want !!!")
endif()

# specify the cross compiler
if (DEFINED USE_CLANG)
  set(CMAKE_C_COMPILER ${ISP_PREFIX}/bin/clang)
  set(CMAKE_ASM_COMPILER ${ISP_PREFIX}/bin/clang)
  set(CMAKE_CXX_COMPILER ${ISP_PREFIX}/bin/clang)
  include_directories(${ISP_PREFIX}/riscv32-unknown-elf/include)
else()
  set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
  set(CMAKE_ASM_COMPILER riscv32-unknown-elf-gcc)
  set(CMAKE_CXX_COMPILER riscv32-unknown-elf-g++)
endif()
#CMAKE_FORCE_C_COMPILER(riscv64-unknown-elf-gcc GNU)
#CMAKE_FORCE_CXX_COMPILER(riscv64-unknown-elf-g++ GNU)

#set(CMAKE_EXE_LINKER_FLAGS "-static -mabi=ilp32 -march=rv32g")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBRARIES OFF)

set(CMAKE_FIND_ROOT_PATH  ${ISP_PREFIX}/riscv32-unknown-elf)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# baseline compiler flags:
#   make sure we do 32bit
#   software floating point only
#set(CMAKE_C_FLAGS "-mabi=ilp32 -march=rv32g -msoft-float -Os" CACHE STRING "" FORCE)
#set(CMAKE_ASM_FLAGS "-mabi=ilp32 -march=rv32g -msoft-float" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os" CACHE STRING "" FORCE)
#set(CMAKE_ASM_FLAGS "-mabi=ilp32 -march=rv32g" CACHE STRING "" FORCE)
