
hello_world demo for Free-RTOS

Set the following Env vars:

export FREE_RTOS_DIR=<path to>/FreeRTOS

this project is set up with cmake to build:

'''
cd hello_world
mkdir build
cd build
FREE_RTOS_DIR=<path to base freertos> CC=<path to clang crosscompiler> cmake ..
make
...

