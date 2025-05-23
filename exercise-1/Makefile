all: build build/server build/client

build/server: src/tcp_echo_server.cc
	g++ src/tcp_echo_server.cc -o build/server

build/client: src/tcp_echo_client.cc
	g++ src/tcp_echo_client.cc -o build/client

build:
	mkdir build

.PHONY: clean

clean:
	rm -r build
