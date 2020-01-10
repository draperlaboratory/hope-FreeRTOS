#!/bin/bash
cd FreeRTOS/Demo/RISC-V_Galois_P1/
# Set the sysroot path for LLVM
export C_INCLUDE_PATH=$RISCV_C_INCLUDE_PATH
for use_clang in yes no
do
    export USE_CLANG=$use_clang
    for prog in main_blinky main_full main_udp main_tcp
    do
        make clean
        export PROG=$prog
        make
        if [ $? -eq 0 ]
        then
            echo USE_CLANG=$USE_CLANG $PROG OK
        else
            echo USE_CLANG=$USE_CLANG $PROG failed
            exit 1
        fi
    done
done
make clean
cd -
