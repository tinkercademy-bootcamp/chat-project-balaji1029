#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include <map>
#include <optional>

#include <spdlog/spdlog.h>

#include "../net/chat-sockets.h"
#include "../utils.h"
#include "chat-server.h"

#define MAX_EVENTS 32

tt::chat::server::Server::Server(int port)
    : socket_(tt::chat::net::create_socket()),
      address_(tt::chat::net::create_address(port)) {
  using namespace tt::chat;
  set_socket_options(socket_, 1);

  channels.push_back(Channel("Default"));

  address_.sin_addr.s_addr = INADDR_ANY;

  auto err_code = bind(socket_, (sockaddr *)&address_, sizeof(address_));
  check_error(err_code < 0, "bind failed\n");
  
  check_error(fcntl(socket_, F_SETFL, fcntl(socket_, F_GETFL, 0) | O_NONBLOCK) == -1, "Non-blocking error");

  err_code = listen(socket_, 3);
  check_error(err_code < 0, "listen failed\n");

  std::cout << "Server listening on port " << port << "\n";
}

tt::chat::server::Server::~Server() { close(socket_); }

void tt::chat::server::Server::handle_connections() {
  socklen_t address_size = sizeof(address_);

  struct epoll_event events[MAX_EVENTS];

  int epfd;
  
  check_error((epfd = epoll_create1(EPOLL_CLOEXEC)) == -1, "epoll_create1 failed");

  struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = socket_;
  
  // check_error(epoll_ctl(epfd, EPOLL_CTL_ADD, socket_, &ev) == -1, "epoll_ctl error\n");

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, socket_, &ev) == -1) {
    perror("epoll_ctl");
    throw std::runtime_error("epoll_ctl error");
  }

  while (true) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);

    for (int i=0; i < nfds; i++) {
      if (events[i].data.fd == socket_) {
        int accepted_socket = accept(socket_, (sockaddr *)&address_, &address_size);
        tt::chat::check_error(accepted_socket < 0, "Accept error n ");
        std::string username = handle_accept(accepted_socket, true).value_or("");
        if (username == "username") {
          close(accepted_socket);
          continue;
        }
        usernames[accepted_socket] = username;

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = accepted_socket;
        // check_error(fcntl(accepted_socket, F_SETFL, fcntl(accepted_socket, F_GETFL, 0) | O_NONBLOCK) == -1, "Non-blocking error");
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, accepted_socket, &ev) == -1) {
          perror("epoll_ctl");
          throw std::runtime_error("epoll_ctl error");
        }
      } else {
        handle_accept(events[i].data.fd);
      }
    }
  }
}

void tt::chat::server::Server::set_socket_options(int sock, int opt) {
  using namespace tt::chat;
  auto err_code = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                             &opt, sizeof(opt));
  check_error(err_code < 0, "setsockopt() error\n");
}

std::optional<std::string> tt::chat::server::Server::handle_accept(int sock, bool first_message) {
  using namespace tt::chat;

  std::optional<std::string> mesg;

  char buffer[kBufferSize] = {0};
  ssize_t read_size = read(sock, buffer, kBufferSize);

  if (read_size > 0) {
    mesg = buffer;
    if (!first_message) {
      SPDLOG_INFO("Received from {}: {}", usernames[sock], buffer);
      send(sock, buffer, read_size, 0);
      SPDLOG_INFO("Echo message sent");
    } else {
      for (auto username: usernames) {
        if (username.second == buffer) {
          send(sock, "unavailable", sizeof("unavailable"), 0);
          SPDLOG_INFO("{} tried to connect again", buffer);
          return "unavailable";
        }
      }
      send(sock, buffer, read_size, 0);
      SPDLOG_INFO("{} connected", buffer);
    }
  } else if (read_size == 0) {
    close(sock);
    SPDLOG_INFO("{} disconnected.", usernames[sock]);
  } else {
    SPDLOG_ERROR("Read error on client socket {}", socket_);
  }
  // close(sock);
  return mesg;
}