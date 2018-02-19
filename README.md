# FreeRTOS-RISCV

Version 10.0.0 of Amazon FreeRTOS under MIT license

This project has the RISCV port of FreeRTOS and the MIV board support code.

There is a hello_world demo project that implements a uart management task, and two tasks that periodically write the uart.

See: Demo/RISCV_MIV_GCC/hello_world

Directories:

+ The FreeRTOS/Source directory contains the FreeRTOS source code, and contains
  its own readme file.

+ This version of FreeRTOS/Demo directory contains only the RISCV
port. Download the full version to get other ports.

+ See http://www.freertos.org/a00017.html for full details of the directory 
  structure and information on locating the files you require.

The easiest way to use FreeRTOS is to start with one of the pre-configured demo 
application projects (found in the FreeRTOS/Demo directory).  That way you will
have the correct FreeRTOS source files included, and the correct include paths
configured.  Once a demo application is building and executing you can remove
the demo application file, and start to add in your own application source
files.

See also -
http://www.freertos.org/FreeRTOS-quick-start-guide.html
http://www.freertos.org/FAQHelp.html
