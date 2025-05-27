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
    void connect_to_server(int sock, sockaddr_in &server_address);
    void send_and_receive_message(int sock, const std::string &message);
  };

} // namespace tt::chat::client

#endif // CHAT_CLIENT_H