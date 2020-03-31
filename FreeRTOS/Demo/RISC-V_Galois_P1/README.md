<<<<<<< HEAD
# TODO
Add something sensible to readme
=======
# RISC-V Galois P1
Set of tests for BESSPIN P1 processors. See [BESSPIN-GFE-2019](https://github.com/GaloisInc/BESSPIN-GFE-2019) for more details. These tests are a part of the test suite for `gfe` MCUs.

These tests don't need any additional hardware:
* `main_blinky` "blinks" to the UART to show scheduler runs
* `main_full` standard full-stack FreeRTOS demonstration

These tests require additional hardware:
* `main_iic` smoketest on i2c interface
* `main_gpio` toggles GPIO pins
* `main_uart` outputs and reads from both UARTs
* `main_sd` write and read from an SD card
* `main_udp` UDP echo server and client
* `main_tcp` TCP echo server and client
>>>>>>> gdevelop
