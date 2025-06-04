#include "chat-client.h"
#include "../net/chat-sockets.h"
#include "../utils.h"
#include <atomic>
#include <mutex>
#include <ncurses.h>

#define MAX_EVENTS 32

tt::chat::client::Client::Client(int port,
                                         const std::string &server_address)
    : socket_{tt::chat::net::create_socket()} {
  running.store(true);
  sockaddr_in address = create_server_address(server_address, port);
  connect_to_server(socket_, address);
  selected_channel = 0;
  mode = CHOICE;
  input_pos = 0;
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
  std::lock_guard<std::mutex> lock(channel_mutex);
  channel_names.push_back(channel_name);
}

int tt::chat::client::Client::get_channel_count() {
  std::lock_guard<std::mutex> lock(channel_mutex);
  return channel_names.size();
}

std::string tt::chat::client::Client::get_channel_by_id(const int &id) {
  std::lock_guard<std::mutex> lock(channel_mutex);
  if (id >= 0 && id < static_cast<int>(channel_names.size())) {
    return channel_names.at(id);
  }
  return "";
}

std::string tt::chat::client::Client::receive_message() {
  using namespace tt::chat;
  char recv_buffer[kBufferSize] = {0};

  // Receive response from the server
  ssize_t read_size = read(socket_, recv_buffer, kBufferSize - 1);
  if (read_size > 0) {
    recv_buffer[read_size] = '\0';  // Ensure null termination
    return std::string(recv_buffer);
  } else if (read_size == 0) {
    return SERVER_ERROR;
  } else {
    return READ_ERROR;
  }
}

void tt::chat::client::Client::receive_thread(tt::chat::client::Window &input_win, tt::chat::client::Window &chat_win, tt::chat::client::Window &channel_win) {
  while (running.load()) {
    std::string message = receive_message();
    
    // Check for connection errors
    if (message == SERVER_ERROR || message == READ_ERROR) {
      running.store(false);
      break;
    }

    if (message[0] == 'k') break;
    
    // Only process incoming messages, don't echo them back
    if (message[0] == 'c') {
      // New channel created - add it to our list
      push_channel_name(message.substr(2, message.size()-2));
    } else if (message[0] == 'm') {
      // Incoming chat message - parse and display
      message = message.substr(2, message.size()-2);
      size_t first_colon = message.find_first_of(':');
      if (first_colon != std::string::npos) {
        std::string channel_num = message.substr(0, first_colon);
        message = message.substr(first_colon+1, message.size()-first_colon-1);
        if (std::stoi(channel_num) != current_channel) continue;
        size_t second_colon = message.find_first_of(':');
        if (second_colon != std::string::npos) {
          std::string user = message.substr(0, second_colon);
          message = message.substr(second_colon+1, message.size()-second_colon-1);
          chats.push_back({user, message});
        }
      }
    }

    if (mode == INPUT) curs_set(1);
    else curs_set(0);

    channel_win.erase();
    channel_win.boxit(0, 0);
    channel_win.print(0, 2, (mode == CHANNELS) ? " Channels [F] " : " Channels ");

    if (selected_channel == -1) {
      channel_win.wattr(true);
    }
    // mvwprintw(channel_win, 1, 1, "New Channel");
    channel_win.print(1, 1, "New Channel");
    channel_win.wattr(false);
    for (int i = 0; i < get_channel_count(); i++) {
      if (i == selected_channel) {
        channel_win.wattr(true);
      }
      // mvwprintw(, i+2, 1, "%s", get_channel_by_id(i).c_str());
      channel_win.print(i+2, 1, get_channel_by_id(i));
      channel_win.wattr(false);
    }

    // Draw chat box
		// werase(chat_win);
    chat_win.erase();
		// box(chat_win, 0, 0);
    chat_win.boxit(0, 0);
		// mvwprintw(chat_win, 0, 2, (std::string(" ") + get_channel_by_id(current_channel) + " " + ((mode == CHAT) ? "[F] " : "")).c_str());
    chat_win.print(0, 2, " " + get_channel_by_id(current_channel) + " " + ((mode == CHAT) ? "[F] " : ""));
    // mvwprintw(chat_win, 1, 1, "Key: %d", key);
    for (int i=0; i<chats.size(); i++) {
      // mvwprintw(chat_win, i+1, 1, "%s", (chats[i].user + "\t:  " + chats[i].message).c_str());
      chat_win.print(i+1, 1, chats[i].user + "\t:  " + chats[i].message);
    }

    // Draw input
		// werase(input_win);
		// box(input_win, 0, 0);
		// mvwprintw(input_win, 0, 2, " Input (%s mode) ", mode == INPUT ? "Insert" : "Nav");
		// mvwprintw(input_win, 1, 1, "%s", input_string.c_str());

    input_win.erase();
    input_win.boxit(0, 0);
    input_win.print(0, 2, " Input (" + std::string((mode == INPUT) ? "Insert" : "Nav") + " mode) ");

    if (mode == INPUT) {
			// wmove(input_win, 1, 1 + input_pos);
      input_win.move_cursor(1, 1+input_pos);
		}

    // Refresh windows
    channel_win.refresh();
    chat_win.refresh();
    input_win.refresh();
    // Note: Removed the automatic send_message(message) that was causing echo
  }
}

tt::chat::client::Client::~Client() { 
  running.store(false);
  close(socket_); 
}

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