uArm-Controller
---
PlatformIO project for the UFACTORY uArm Controller.

## Device Profile FSM
Following figure shows the possible states and transitions of a device profile:
![FSM](https://user-images.githubusercontent.com/26181/98770955-a18f3f00-23e3-11eb-87cb-f92db472a9bf.jpg)
- `UNREG`: The profile is created, but not registered/initialized yet
- `Idle`: The profile is in idle mode, ready for a new request.
- `BLOCKING`: The profile is blocking all other profiles until a response gets received.
- `WAITING`: The profile is waiting for an event, all other profiles are not blocked.

# Synopsis
To compile the proto files, you need to install [protoc](https://grpc.io/docs/protoc-installation/) and [nanopb_generator](https://pypi.org/project/nanopb/) in your system.

# Example: Controller Test
A simple test application on uArm Controller for testing the line protocol + device initialization.
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
- `<new_driver>.cpp` to src/drivers
- `<new_driver>.h` to src/drivers
- switch cases inside `main.cpp`: `action_hanlder()` and `registration_handler()`
- include line in `main.h`
- message skeleton in `line_protocol.proto`
- class skeleton in `simple_gateway.py`

# Caution
The labeling on the backside of the UFactory controller is not correct, IIC and digital ports are swapped. To connect an IIC device, use port 1 or port 2 indicated in the [uArm Controller user manual](https://cdn.shopify.com/s/files/1/0012/6979/2886/files/uArm_Controller_20190718.pdf?v=1597982243).
