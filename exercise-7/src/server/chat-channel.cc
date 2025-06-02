#include <spdlog/spdlog.h>

#include "../net/chat-sockets.h"
#include "../utils.h"
#include "chat-channel.h"

tt::chat::server::Channel::Channel(std::string name) : name(name) {}

std::string tt::chat::server::Channel::get_name() {return name;}

int tt::chat::server::Channel::add_user(int fd) {
  auto fd_ptr = users_fds.find(fd);
  if (fd_ptr != users_fds.end()) {
    SPDLOG_INFO("User already is in the channel");
    return -1;
  }
  users_fds.insert(fd);
  return 0;
}

int tt::chat::server::Channel::remove_user(int fd) {
  auto fd_ptr = users_fds.find(fd);
  if (fd_ptr != users_fds.end()) {
    users_fds.erase(fd_ptr);
    return 0;
  }
  SPDLOG_INFO("User doesn't exist in the channel");
  return -1;
}