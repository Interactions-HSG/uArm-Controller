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
Use `make run` to build and upload firmware + run `controller_test.py` script

# Automatic Driver Initialization
To add a new Driver named <new_driver> run following command:
```
% cd uArm-Controller && make add_driver driver_name=<new_driver>
```
This will add following template files/sections:
- <new_driver>.cpp to src/drivers
- <new_driver>.h to include/drivers
- template switch cases inside main::action_hanlder(), main::registration_handler() => NOT IMPLEMENTED YET