#!/bin/bash

# build + upload firmware => then run .py script

# TODO: figure out how to save all open files
platformio run --target upload
python ./examples/simple_gateway.py COM3
