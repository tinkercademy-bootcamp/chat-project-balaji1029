#include "chat-client.h"
#include "../net/chat-sockets.h"
#include "../utils.h"

#define MAX_EVENTS 32

tt::chat::client::Client::Client(int port,
                                         const std::string &server_address)
    : socket_{tt::chat::net::create_socket()}, running(true) {
  sockaddr_in address = create_server_address(server_address, port);
  connect_to_server(socket_, address);
}

std::string tt::chat::client::Client::send_and_receive_message(
    const std::string &message) {
  using namespace tt::chat;

  // Send the message to the server
  send_message(message);

  // Receive response from the server
  return receive_message();
}

int tt::chat::client::Client::send_message(const std::string& message) {
  return send(socket_, message.c_str(), message.size()+1, 0) > 0 ? 0 : -1;
}

void tt::chat::client::Client::push_channel_name(const std::string &channel_name) {
  channel_names.push_back(channel_name);
}

int tt::chat::client::Client::get_channel_count() {
  return channel_names.size();
}

std::string tt::chat::client::Client::get_channel_by_id(const int &id) {
  return channel_names.at(id);
}

std::string tt::chat::client::Client::receive_message() {
  using namespace tt::chat;
  char recv_buffer[kBufferSize] = {0};

  // Receive response from the server
  ssize_t read_size = read(socket_, recv_buffer, kBufferSize);
  if (read_size > 0) {
    return std::string(recv_buffer);
  } else if (read_size == 0) {
    return SERVER_ERROR;
  } else {
    return READ_ERROR;
  }
}

void tt::chat::client::Client::receive_thread() {

  while (running) {
    // int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    std::string message = receive_message();
    send_message(message);

    if (message[0] == 'c') {
      push_channel_name(message.substr(2, message.size()-2));
    } else if (message[0] == 'm') {
      int first_colon = message.find_first_of(':');
      std::string cut_message = message.substr(first_colon + 1, message.size() - first_colon - 1);
      int second_colon = cut_message.find_first_of(':');
      std::string user = cut_message.substr(0, second_colon);
      std::string actual_message = cut_message.substr(second_colon + 1, cut_message.size() - second_colon - 1);
      chats.push_back({user, message});
    }
  }
}

tt::chat::client::Client::~Client() { close(socket_); }

sockaddr_in tt::chat::client::Client::create_server_address(
    const std::string &server_ip, int port) {
  using namespace tt::chat;
  sockaddr_in address = net::create_address(port);
  // Convert the server IP address to a binary format
  auto err_code = inet_pton(AF_INET, server_ip.c_str(), &address.sin_addr);
  check_error(err_code <= 0, "Invalid address/ Address not supported\n");
  return address;
}

void tt::chat::client::Client::connect_to_server(
    int sock, sockaddr_in &server_address) {
  using namespace tt::chat;
  auto err_code =
      connect(sock, (sockaddr *)&server_address, sizeof(server_address));
  check_error(err_code < 0, "Connection Failed.\n");
}
