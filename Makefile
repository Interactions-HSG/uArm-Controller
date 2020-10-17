# See README.md.

.PHONY: protobuf clean

all: example

example:
	# for the example
	protoc -I=./protobuf --python_out=./examples ./protobuf/line_protocol.proto
	nanopb_generator --strip-path --output-dir=./include --timestamp ./protobuf/line_protocol.proto

clean:
	# for the example
	rm -f examples/line_protocol_pb2.py
	rm -f include/protobuf/line_protocol.pb.h
	rm -f src/line_protocol.pb.c

# Automatic driver initialization
# TODO: add string manipulation => add code snippets in line_protocol.proto, main.pp: action_handler/ registration_handler
add_driver: 
	# add <new_driver>.cpp to src/drivers
	sed 's/template_driver/$(driver_name)/' ./tools/template_files/template_driver.cpp > ./src/drivers/$(driver_name).cpp
	# <new_driver>.h to include/drivers
	sed 's/template_driver/$(driver_name)/' ./tools/template_files/template_driver.h > ./include/drivers/$(driver_name).h
	
	#DRIVER_NAME= echo ${driver_name}| tr '[a-z]' '[A-Z]' 

# build + upload firmware => then run .py script
run:
	# TODO: figure out how to save all open files
	platformio run --target upload
	python ./examples/controller_test.py COM3
