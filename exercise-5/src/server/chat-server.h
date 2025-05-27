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

    public:
    Server(int port): port(port){}

    void set_socket_options(int opt);
    int create_server_socket();
    void bind_address_to_socket();
    void listen_on_socket();
    void handle_accept(int socket);
    sockaddr_in create_server_address();
    void handle_connections();
  };

} // namespace tt::chat::server