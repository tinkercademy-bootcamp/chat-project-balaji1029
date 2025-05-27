#include "server/chat-server.h"

int main() {
  namespace ttc = tt::chat;
  const int kPort = 8080;

  ttc::server::Server server = ttc::server::Server(kPort);

  int my_socket = server.create_server_socket();
  sockaddr_in address = server.create_server_address();

  // start listening on the socket
  server.bind_address_to_socket();
  server.listen_on_socket();

  std::cout << "Server listening on port " << kPort << "\n";
  server.handle_connections();
  close(my_socket);

  return 0;
}
