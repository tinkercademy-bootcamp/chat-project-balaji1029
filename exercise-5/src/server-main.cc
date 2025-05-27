#include "server/chat-server.h"

int main() {
  namespace ttc = tt::chat;
  const int kPort = 8080;

  ttc::server::Server server = ttc::server::Server(kPort);

  // start listening on the socket
  server.keep_listening();

  return 0;
}
