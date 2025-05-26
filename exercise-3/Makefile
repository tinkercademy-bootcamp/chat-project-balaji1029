CXX_FLAGS := -fsanitize=address
LD_FLAGS := -fsanitize=address

all: build build/server build/client

# build/server: src2/tcp_echo_server.cc
# 	g++ $(CXX_FLAGS) $< -o $@

# build/client: src2/tcp_echo_client.cc
# 	g++ $(CXX_FLAGS) $< -o $@

build/%: src/tcp_echo_%.cc
		g++ $(CXX_FLAGS) $< -o $@

build:
	mkdir build

.PHONY: clean

clean:
	rm -r build
