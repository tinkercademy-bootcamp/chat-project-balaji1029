#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <netinet/in.h>
#include <string>
#include <map>
#include <optional>

namespace tt::chat::server {

class Server {
public:
  Server(int port);
  ~Server();
  void handle_connections();

private:
  int socket_;
  sockaddr_in address_;
  std::map<int, std::string> usernames;

  static constexpr int kBufferSize = 1024;

  std::optional<std::string> handle_accept(int sock, bool first_message = false);
  static void set_socket_options(int sock, int opt);
};
} // namespace tt::chat::server

#endif // CHAT_SERVER_H
