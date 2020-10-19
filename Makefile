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



