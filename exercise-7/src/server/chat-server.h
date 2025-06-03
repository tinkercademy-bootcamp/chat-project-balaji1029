#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <netinet/in.h>
#include <string>
#include <map>
#include <optional>
#include <vector>

#include "chat-channel.h"

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
  std::vector<Channel> channels;
  std::map<int, int> user_to_channel;

  static constexpr int kBufferSize = 1024;

  std::string handle_initial_connection(int sock);
  void send_channel_info(int sock);
  void handle_client_message(int sock);
  void handle_client_disconnect(int sock);
  void broadcast_to_channel(int channel_id, const std::string& message);
  void broadcast_to_all_users(const std::string& message);
  std::optional<std::string> handle_accept(int sock, bool first_message = false);
  static void set_socket_options(int sock, int opt);
  int send_message(int sock, std::string message);
  std::string receive_message(int sock);
};
} // namespace tt::chat::server

#endif // CHAT_SERVER_H
