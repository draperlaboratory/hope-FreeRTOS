# Web Server Demo

*NOTE:* There are two approaches to interfacing with the webserver:
1. Tap interface (requires root access and a physical network interface, but is faster)
2. User mode networking (more portable, no root access required, but is slower)

## Setup (Tap interface)

#### Setting up the network interfaces

1. Run `ifconfig` and identify the name of an available network interface (eg. eth0).
2. Change the `ETH_IF` variable in `tap_setup.sh` and `tap_teardown.sh` to the chosen interface.
3. Run `tap_setup.sh`. If successful, the only output should be `Set 'tap0' persistent and owned by uid <uid>`.
4. When finished running the webapp demo, restore your network state by running `tap_teardown.sh`. If successful, the only output should be `Set 'tap0' nonpersistent`.

#### Running the webserver
1. `make`
2. `make sim-qemu-tap POLICY=<policy>`
3. To view UART output from qemu, run `tail -f build/isp-run-FreeRTOS-web-server.elf-<policy>`.
4. Wait until the line `CGI Setup complete` appears in the UART output. This indicates that the webserver is fully initialized.
5. Open a browser and navigate to `http://172.25.218.200/login.html`.

## Setup (User networking)

#### Running the webserver
1. `make`
2. `make sim-qemu POLICY=<policy>`
3. To view UART output from qemu, run `tail -f build/isp-run-FreeRTOS-web-server.elf-<policy>`.
4. Wait until the line `CGI Setup complete` appears in the UART output. This indicates that the webserver is fully initialized.
5. Open a browser and navigate to `http://127.0.0.1:5555/login.html`.
