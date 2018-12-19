#!/bin/bash
COMPILER=riscv32-unknown-elf-gcc
ARCHIVER=riscv32-unknown-elf-gcc-ar

cd libsrc/standalone_v5_5/src
make

cd ../../emacps_v3_2/src
make

# Replace these
cd ../../scutimer_v2_1/src
make

cd ../../scugic_v3_3/src
make
