#ifndef CHAT_CHANNEL_H
#define CHAT_CHANNEL_H

#include <netinet/in.h>
#include <set>

namespace tt::chat::server {

class Channel {
  std::set<int> users_fds;

public:
  Channel(int fd);
  int add_user(int fd);
  int remove_user(int fd);
};
} // namespace tt::chat::server

#endif // CHAT_CHANNEL_H