all: build build/server build/client

build/server: src2/tcp_echo_server.cc
	g++ $< -o $@

build/client: src2/tcp_echo_client.cc
	g++ $< -o $@

build:
	mkdir build

.PHONY: clean

clean:
	rm -r build
