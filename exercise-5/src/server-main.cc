#include "server/chat-server.h"

int main() {
  namespace ttc = tt::chat;
  const int kPort = 8080;

  ttc::server::Server server = ttc::server::Server(kPort);

  int my_socket = server.create_server_socket();
  sockaddr_in address = server.create_server_address();

  // start listening on the socket
  server.bind_address_to_socket(my_socket, address);
  server.listen_on_socket(my_socket);

  std::cout << "Server listening on port " << kPort << "\n";
  server.handle_connections(my_socket, address);
  close(my_socket);

  return 0;
}
