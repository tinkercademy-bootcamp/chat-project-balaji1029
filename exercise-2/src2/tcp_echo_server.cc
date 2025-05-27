#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int connect_server() {
  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Socket creation erron\n";
    exit(EXIT_FAILURE);
  }
  return sock;
}

void attach_port(int sock, int &opt) {
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    std::cerr << "setsockopt error\n";
    exit(EXIT_FAILURE);
  }
}

void bind_socket(int sock, sockaddr_in &address, int port) {
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  if (bind(sock, (sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << "bind failed\n";
    exit(EXIT_FAILURE);
  }
}

void listen_socket(int sock) {
  if (listen(sock, 3) < 0) {
    std::cerr << "listen failed\n";
    exit(EXIT_FAILURE);
  }
}

void read_and_send(int my_sock, int &new_sock, sockaddr_in &address, socklen_t addrlen, char* buffer, int kBufferSize) {
  new_sock = accept(my_sock, (struct sockaddr *)&address, &addrlen);
  if (new_sock < 0) {
    std::cerr << "accept error\n";
    exit(EXIT_FAILURE);
  }
  // Wait for read
  ssize_t read_size = read(new_sock, buffer, kBufferSize);
  std::cout << "Received: " << buffer << "\n";
  // Send reply
  send(new_sock, buffer, read_size, 0);
  std::cout << "Echo message sent" << "\n";
}

int main() {
  const int kPort = 35000;
  sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  const int kBufferSize = 1024;
  char buffer[kBufferSize] = {0};
  int my_sock;
  int opt = 1;
  
  // Creating socket file descriptor
  my_sock = connect_server();
  
  // Attaching socket to port
  attach_port(my_sock, opt);
  
  // Bind the socket to the network address and port
  bind_socket(my_sock, address, kPort);
  std::cout << "Server listening on port " << kPort << "\n";
  
  // Start listening for incoming connections
  listen_socket(my_sock);

  // Accept incoming connection
  int new_sock;
  while (true) {
    read_and_send(my_sock, new_sock, address, addrlen, buffer, kBufferSize);
  }
  // Close the socket
  close(new_sock);
  close(my_sock);
  return 0;
}