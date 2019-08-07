#!/bin/bash
cd FreeRTOS/Demo/RISC-V_Galois_P1/
for use_clang in no yes
do
    export USE_CLANG=$use_clang
    for prog in main_blinky main_full main_iic main_sd main_uart main_udp main_tcp
    do
        make clean
        export PROG=$prog
        make
        if [ $? -eq 0 ]
        then
            echo $USE_CLANG $PROG OK
        else
            echo $USE_CLANG $PROG failed
            exit 1
        fi
    done
done
make clean
cd -