# See README.md.

.PHONY: protobuf clean

all: example

example:
	# for the example
	protoc -I=./protobuf --python_out=./examples ./protobuf/heartbeat.proto
	nanopb_generator --strip-path --output-dir=./include --timestamp ./protobuf/heartbeat.proto

clean:
	# for the example
	rm -f examples/heartbeat_pb2.py
	rm -f include/protobuf/heartbeat.pb.c
	rm -f include/protobuf/heartbeat.pb.c
