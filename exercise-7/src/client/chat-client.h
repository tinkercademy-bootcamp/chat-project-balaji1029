#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <netinet/in.h>
#include <string>
#include <vector>

#define SERVER_ERROR "Server closed connection.\n"
#define READ_ERROR "Read error.\n"

namespace tt::chat::client {
class Client {
public:
  Client(int port, const std::string &server_address);
  std::string send_and_receive_message(const std::string &message);
  int send_message(const std::string &message);
  std::string receive_message();
  void push_channel_name(const std::string &channel_name);
  int get_channel_count();
  std::string get_channel_by_id(const int& id);
  ~Client();

private:
  int socket_;
  std::vector<std::string> channel_names;

  sockaddr_in create_server_address(const std::string &server_ip, int port);
  void connect_to_server(int sock, sockaddr_in &server_address);

  static constexpr int kBufferSize = 1024;
};
} // namespace tt::chat::client

#endif // CHAT_CLIENT_H
