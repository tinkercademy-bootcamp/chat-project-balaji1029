#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../net/chat-sockets.h"
#include "../utils.h"


namespace tt::chat::server {

  class Server {
    int sock;
    sockaddr_in address;

    void set_socket_options(int sock, int opt);
    int create_server_socket();
    void bind_address_to_socket(int sock, sockaddr_in &address);
    void listen_on_socket(int sock);
    void handle_accept(int sock);

  };
  
  sockaddr_in create_server_address(int port) {
    namespace ttn = tt::chat::net;
    sockaddr_in address = ttn::create_address(port);
    address.sin_addr.s_addr = INADDR_ANY;
    return address;
  }

  void handle_connections(int sock, sockaddr_in &address) {
    socklen_t address_size = sizeof(address);

    while (true) {
      int accepted_socket = accept(sock, (sockaddr *)&address, &address_size);
      check_error(accepted_socket < 0, "Accept error n ");
      handle_accept(accepted_socket);
    }
  }

} // namespace tt::chat::server