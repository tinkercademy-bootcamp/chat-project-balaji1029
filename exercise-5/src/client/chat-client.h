#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../net/chat-sockets.h"
#include "../utils.h"

#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

namespace tt::chat::client {

  class Client {
    sockaddr_in create_server_address(const std::string &server_ip, int port);
  };


void connect_to_server(int sock, sockaddr_in &server_address) {
  using namespace tt::chat;
  auto err_code =
      connect(sock, (sockaddr *)&server_address, sizeof(server_address));
  check_error(err_code < 0, "Connection Failed.\n");
}

void send_and_receive_message(int sock, const std::string &message) {
  using namespace tt::chat;
  const int kBufferSize = 1024;
  char recv_buffer[kBufferSize] = {0};

  // Send the message to the server
  send(sock, message.c_str(), message.size(), 0);
  std::cout << "Sent: " << message << "\n";

  // Receive response from the server
  ssize_t read_size = read(sock, recv_buffer, kBufferSize);
  check_error(read_size < 0, "Read error.\n");
  if (read_size > 0) {
    std::cout << "Received: " << recv_buffer << "\n";
  } else if (read_size == 0) {
    std::cout << "Server closed connection.\n";
  }
}

} // namespace tt::chat::client

#endif // CHAT_CLIENT_H