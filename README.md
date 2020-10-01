uArm-Controller
---
PlatformIO project for the UFACTORY uArm Controller.

# Synopsis
To compile the proto files, you need to install [protoc](https://grpc.io/docs/protoc-installation/) and [nanopb_generator](https://pypi.org/project/nanopb/) in your system.

# Example: Tone Heartbeat
A simple toy application on uArm Controller exchanging two types of PB messages.
```
% cd uArm-Controller && make example
% python3 examples/tone_heartbeat.py <serial port of the controller>
```
