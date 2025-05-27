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

  };


  void bind_address_to_socket(int sock, sockaddr_in &address) {
    namespace ttc = tt::chat;
    auto err_code = bind(sock, (sockaddr *)&address, sizeof(address));
    ttc::check_error(err_code < 0, "bind failed\n");
  }

  void listen_on_socket(int sock) {
    namespace ttc = tt::chat;
    auto err_code = listen(sock, 3);
    ttc::check_error(err_code < 0, "listen failed\n");
  }

  void handle_accept(int sock) {
    namespace ttc = tt::chat;
    const int kBufferSize = 1024;
    char buffer[kBufferSize] = {0};
    ssize_t read_size = read(sock, buffer, kBufferSize);

    ttc::check_error(read_size < 0,
                    "Read error on client socket " + std::to_string(sock));
    if (read_size > 0) {
      std::cout << "Received:" << buffer << "\n";
      send(sock, buffer, read_size, 0);
      std::cout << "Echo message sent\n";
    } else if (read_size == 0) {
      std::cout << "Client disconnected.\n";
    } else {
      std::cerr << "Read error on client socket " << sock << "\n";
    }
    close(sock);
  }

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