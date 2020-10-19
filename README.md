uArm-Controller
---
PlatformIO project for the UFACTORY uArm Controller.

# Synopsis
To compile the proto files, you need to install [protoc](https://grpc.io/docs/protoc-installation/) and [nanopb_generator](https://pypi.org/project/nanopb/) in your system.

# Example: Controller Test
A simple test application on uArm Controller for testing the line protocol + device initialization.
When starting the controller_test.py script the internal R_LED is initialized and periodically activated.
```
% cd uArm-Controller && make example
% python3 examples/controller_test.py <serial port of the controller>
```

# Tools

## Complete Execution
To automatically build + upload the MCU firmware and then run the script `simple_gateway.py`, use the following line:
```
 ./tools/run_controller.sh
```
(The generation of the nanopb files is not included.)

## Automatic Driver Initialization
To add a new Driver named <new_driver> run following command:
```
./tools/driver_init/create_driver.sh <new_driver>
```
This will add following template files/sections:
- <new_driver>.cpp to src/drivers
- <new_driver>.h to include/drivers
- template switch cases inside main::action_hanlder(), main::registration_handler() => NOT IMPLEMENTED YET
