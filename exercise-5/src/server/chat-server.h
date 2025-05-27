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
    int port;

    void set_socket_options(int opt);
    int create_server_socket();
    void bind_address_to_socket();
    void listen_on_socket();
    void handle_accept(int socket);
    sockaddr_in create_server_address();
    void handle_connections();

    public:
    Server(int port): port(port){
      create_server_socket();
      create_server_address();
    }

    void keep_listening() {
      bind_address_to_socket();
      listen_on_socket();

      std::cout << "Server listening on port " << port << "\n";
      handle_connections();
    }

    ~Server() {
      close(sock);
    }

  };

} // namespace tt::chat::server