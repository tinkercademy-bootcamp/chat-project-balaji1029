#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../net/chat-sockets.h"
#include "chat-server.h"
#include "../utils.h"

void tt::chat::server::Server::set_socket_options(int sock, int opt) {
  namespace ttc = tt::chat;
  auto err_code = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                            &opt, sizeof(opt));
  ttc::check_error(err_code < 0, "setsockopt() error\n");
}

int tt::chat::server::Server::create_server_socket() {
  int sock = net::create_socket();
  set_socket_options(sock, 1);
  return sock;
}

void tt::chat::server::Server::bind_address_to_socket(int sock, sockaddr_in &address) {
  namespace ttc = tt::chat;
  auto err_code = bind(sock, (sockaddr *)&address, sizeof(address));
  ttc::check_error(err_code < 0, "bind failed\n");
}