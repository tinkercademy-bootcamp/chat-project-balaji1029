#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

std::string mesg(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "No message to send :(" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string mesg = argv[1];
  return mesg;
}

int create_socket() {
  int domain = AF_INET;
  int type = SOCK_STREAM;
  int protocol = 0;

  int sock = socket(domain, type, protocol);
  if (sock < 0) {
    std::cerr << "Socker creation error\n";
    exit(EXIT_FAILURE);
  }
  return sock;
}

sockaddr_in get_address(std::string server_addr, int port) {
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  if (inet_pton(AF_INET, server_addr.c_str(), &address.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported\n";
    exit(EXIT_FAILURE);
  }
  return address;
}

int main(int argc, char** argv) {
  std::string message = mesg(argc, argv);

  const int kPort = 35000;
  const std::string kServerAddress = "127.0.0.1";
  const int kBufferSize = 1024;
  char buffer[kBufferSize] = {0};
  
  // Creating socket file descriptor
  int my_sock = create_socket();
  
  sockaddr_in address = get_address(kServerAddress, kPort);
  
  // Connect to the server
  if (connect(my_sock, (sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << "Connection Failed\n";
    return -1;
  }
  // Send message
  send(my_sock, message.c_str(), message.size()+1, 0);
  std::cout << "Sent: " << message << "\n";
  // Wait for reply
  ssize_t read_size = read(my_sock, buffer, kBufferSize);
  std::cout << "Received: " << buffer << "\n";
  // Close the socket
  close(my_sock);
  return 0;
}