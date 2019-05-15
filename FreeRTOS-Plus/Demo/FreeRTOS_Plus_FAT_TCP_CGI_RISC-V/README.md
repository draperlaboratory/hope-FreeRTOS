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

## Running the Demos

#### Privilege Escalation (userType policy)
1. Log in with the credentials `patient_user1` and `password1`.
2. On the Dashboard page, click the `Update Info` button.
3. When prompted to enter a new address, copy and paste `payloads/user-type.txt` into the text box.
4. Submit the address and wait to be redirected to the Dashboard page. If the exploit was successful, the current user type should change from `Patient` to `Administrator`.

#### Information Leakage (password policy)
1. Log in with the credentials `patient_user1` and `password1`.
2. On the Dashboard page, click on the link labeled `Schmoe, Joe` in the `Doctor` field of the user's medical data table.
3. After loading the User Details page, append the parameter `&type=debug` to the end of the URL in the address bar, then reload the page.
4. The User Details page should now display information for the user `the_doctor`, including a password value of `password2`.
5. Return to `login.html` and log in using the leaked credentials.

#### Missing Permissions Check (ppac policy)
1. Log in with the credentials `the_doctor` and `password2`.
2. On the Dashboard page, note that the table of patients includes an `Add Record` button. This button is not present on the patient's Dashboard page.
3. Log out, then log in with the credentials `patient_user1` and `password1`.
4. Copy and paste the URL in `payloads/ppac.txt` into the address bar, then load the URL.
5. Add a medical record and click `Submit`. The page will redirect to the patient's Dashboard, which displays the new medical record.

#### Stack buffer overflow (stack policy)
1. Log in with the credentials `patient_user1` and `password1`.
2. On the Dashboard page, click `Find a Doctor`.
3. On the Search page, enter in some search parameters and click `Search`.
4. To demonstrate the stack overflow, copy and paste the URL in `payloads/stack.txt` into the address bar and load the URL.
5. The Search Results page should reload, returning 0 results. Navigate back to `login.html`.
6. Log in with the credentials `misterhacker` and `password`.

## Running the Negative Tests

The negative tests simulate the behavior of the above exploits automatically. To run them:

1. `make NEGATIVE_TEST=<Password/UserType/UserTypeOverwrite/PPACDoctor/PPACPatient/Stack>`
2. `make sim-qemu`
