#ifndef CHAT_CHANNEL_H
#define CHAT_CHANNEL_H

#include <netinet/in.h>
#include <set>
#include <string>

namespace tt::chat::server {

class Channel {
  std::string name;
  std::set<int> users_fds;

public:
  Channel(std::string name);
  int add_user(int fd);
  int remove_user(int fd);
};
} // namespace tt::chat::server

#endif // CHAT_CHANNEL_H