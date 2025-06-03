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

  // Create a default channel
  channels.push_back(Channel("Default"));

  address_.sin_addr.s_addr = INADDR_ANY;

  auto err_code = bind(socket_, (sockaddr *)&address_, sizeof(address_));
  check_error(err_code < 0, "bind failed\n");
  
  // Make the socket non-blocking
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
  
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, socket_, &ev) == -1) {
    perror("epoll_ctl");
    throw std::runtime_error("epoll_ctl error");
  }

  while (true) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);

    for (int i=0; i < nfds; i++) {
      if (events[i].data.fd == socket_) {
        // New connection - handle initial setup
        int accepted_socket = accept(socket_, (sockaddr *)&address_, &address_size);
        tt::chat::check_error(accepted_socket < 0, "Accept error");
        
        fcntl(accepted_socket, F_SETFL, fcntl(accepted_socket, F_GETFL, 0) | O_NONBLOCK);

        // Handle initial username setup
        std::string username = handle_initial_connection(accepted_socket);
        
        if (username == "unavailable") {
          close(accepted_socket);
          continue;
        }
        
        // Store user info
        usernames[accepted_socket] = username;
        user_to_channel[accepted_socket] = 0; // Default channel
        channels[0].add_user(accepted_socket);

        // Send channel information to new client
        send_channel_info(accepted_socket);

        // Add to epoll for future message handling
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = accepted_socket;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, accepted_socket, &ev) == -1) {
          perror("epoll_ctl");
          throw std::runtime_error("epoll_ctl error");
        }
      } else {
        // Handle message from existing client
        handle_client_message(events[i].data.fd);
      }
    }
  }
}

std::string tt::chat::server::Server::handle_initial_connection(int sock) {
  using namespace tt::chat;
  
  std::string username = receive_message(sock);
  if (username.empty()) {
    return "unavailable";
  }

  // Check if username is already taken
  for (auto& [socket, existing_username] : usernames) {
    if (existing_username == username) {
      send_message(sock, "unavailable");
      SPDLOG_INFO("{} tried to connect but username taken", username);
      return "unavailable";
    }
  }

  // Username is available
  send_message(sock, username);
  SPDLOG_INFO("{} connected", username);
  return username;
}

void tt::chat::server::Server::send_channel_info(int sock) {
  // Send number of channels
  int channel_num = channels.size();
  std::string channel_num_str = std::to_string(channel_num);
  send_message(sock, channel_num_str);
  receive_message(sock); // Wait for acknowledgment

  // Send each channel name
  for (Channel& channel : channels) {
    send_message(sock, channel.get_name());
    SPDLOG_INFO("Channel {} sent to user {}", channel.get_name(), usernames[sock]);
    receive_message(sock); // Wait for acknowledgment
  }
}

void tt::chat::server::Server::handle_client_message(int sock) {
  using namespace tt::chat;

  std::string message = receive_message(sock);
  
  if (message.empty()) {
    // Client disconnected
    handle_client_disconnect(sock);
    return;
  }

  SPDLOG_INFO("Received from {}: {}", usernames[sock], message);

  if (message[0] == 'c') {
    // Create new channel: c:<channel-name>
    std::string channel_name = message.substr(2);
    int channel_id = channels.size();
    channels.push_back(Channel(channel_name));
    channels[channel_id].add_user(sock);
    
    // Move user from current channel to new one
    channels[user_to_channel[sock]].remove_user(sock);
    user_to_channel[sock] = channel_id;

    // Notify all users about new channel
    broadcast_to_all_users("c:" + channel_name);
    
    SPDLOG_INFO("Channel {} created by {}", channel_name, usernames[sock]);

  } else if (message[0] == 'm') {
    // Send message: m:<channel-id>:<message>
    size_t first_colon = message.find(':', 2);
    if (first_colon != std::string::npos) {
      int channel_id = std::stoi(message.substr(2, first_colon - 2));
      std::string actual_message = message.substr(first_colon + 1);
      
      // Broadcast message to all users in the channel
      std::string formatted_message = "m:" + std::to_string(channel_id) + ":" + usernames[sock] + ":" + actual_message;
      broadcast_to_channel(channel_id, formatted_message);
      
      SPDLOG_INFO("Message from {} in channel {}: {}", usernames[sock], channel_id, actual_message);
    }

  } else if (message[0] == 't') {
    // Switch channel: t:<channel-id>
    int new_channel_id = std::stoi(message.substr(2));
    if (new_channel_id >= 0 && new_channel_id < channels.size()) {
      // Remove from current channel
      channels[user_to_channel[sock]].remove_user(sock);
      
      // Add to new channel
      channels[new_channel_id].add_user(sock);
      user_to_channel[sock] = new_channel_id;
      
      SPDLOG_INFO("{} switched to channel {}", usernames[sock], new_channel_id);
    }
  }
}

void tt::chat::server::Server::handle_client_disconnect(int sock) {
  if (usernames.find(sock) != usernames.end()) {
    channels[user_to_channel[sock]].remove_user(sock);
    SPDLOG_INFO("{} disconnected.", usernames[sock]);
    usernames.erase(sock);
    user_to_channel.erase(sock);
  }
  close(sock);
}

void tt::chat::server::Server::broadcast_to_channel(int channel_id, const std::string& message) {
  if (channel_id >= 0 && channel_id < channels.size()) {
    for (int user_sock : channels[channel_id].get_users()) {
      send_message(user_sock, message);
    }
  }
}

void tt::chat::server::Server::broadcast_to_all_users(const std::string& message) {
  for (auto& [sock, username] : usernames) {
    send_message(sock, message);
  }
}

void tt::chat::server::Server::set_socket_options(int sock, int opt) {
  using namespace tt::chat;
  auto err_code = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                             &opt, sizeof(opt));
  check_error(err_code < 0, "setsockopt() error\n");
}

int tt::chat::server::Server::send_message(int sock, const std::string message) {
  return (send(sock, message.c_str(), message.size() + 1, 0) > 0) ? 0 : -1;
}

std::string tt::chat::server::Server::receive_message(int sock) {
  char buffer[kBufferSize];
  ssize_t read_size = read(sock, buffer, kBufferSize - 1);
  if (read_size > 0) {
    buffer[read_size] = '\0';  // Ensure null termination
    return std::string(buffer);
  } else if (read_size == 0) {
    // Client disconnected
    return "";
  } else {
    // Read error (would block or actual error)
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // No data available right now (normal for non-blocking)
      return "";
    }
    SPDLOG_ERROR("Read error on socket {}: {}", sock, strerror(errno));
    return "";
  }
}