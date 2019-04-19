# Web Server Demo

## Setup

#### Setting up the network interfaces

1. Run `ifconfig` and identify the name of an available network interface (eg. eth0).
2. Change the `ETH_IF` variable in `tap_setup.sh` and `tap_teardown.sh` to the chosen interface.
3. Run `tap_setup.sh`. If successful, the only output should be `Set 'tap0' persistent and owned by uid <uid>`.
4. When finished running the webapp demo, restore your network state by running `tap_teardown.sh`. If successful, the only output should be `Set 'tap0' nonpersistent`.

#### Running the webserver
1. `make`
2. `make sim-qemu POLICY=<policy>`
3. To view UART output from qemu, run `tail -f isp-run-FreeRTOS-web-server.elf-<policy>`.
4. Wait until the line `CGI Setup complete` appears in the UART output. This indicates that the webserver is fully initialized.
5. Open a browser and navigate to `http://172.25.218.200/login.html`.
