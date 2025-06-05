#include "chat-client.h"
#include "../net/chat-sockets.h"
#include "../utils.h"
#include <atomic>
#include <mutex>
#include <ncurses.h>
#include <locale.h>

#define LEFT_WIDTH 35
#define INPUT_HEIGHT 3
#define MAX_LINES 1000

#define MAX_EVENTS 32

tt::chat::client::Client::Client(int port,
                                         const std::string &server_address, const std::string& username)
    : socket_{tt::chat::net::create_socket()}, username(username) {
  running.store(true);
  sockaddr_in address = create_server_address(server_address, port);
  connect_to_server(socket_, address);

  std::string response = send_and_receive_message(username);

  if (response == "unavailable") {
    SPDLOG_ERROR("Username {} taken", username);
    exit(EXIT_FAILURE);
  }

  std::string channels_str = receive_message();
  
  while (channels_str.find_first_of(';') != std::string::npos) {
    int semi_colon_index = channels_str.find_first_of(';');
    push_channel_name(channels_str.substr(0, semi_colon_index));
    channels_str = channels_str.substr(semi_colon_index + 1, channels_str.size()-semi_colon_index-1);
  }
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

void tt::chat::client::Client::receive_thread() {
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
      {
        std::lock_guard lock{channel_mutex};
        push_channel_name(message.substr(2, message.size()-2));
      }
    } else if (message[0] == 'm') {
      // Incoming chat message - parse and display
      {
        std::lock_guard lock{chat_mutex};
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
            scroll_offset = 0;
          }
        }
      }
    }
    {
      std::lock_guard lock{refresh_mutex};
      refresh_windows();
    }
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

void tt::chat::client::Client::take_choice_input(const int& key) {
  if (key == 'w') {
    mode = CHANNELS;
  } else if (key == 'c') {
    mode = CHAT;
  } else if (key == 'i') {
    mode = INPUT;
    curs_set(1);
  }
}

void tt::chat::client::Client::take_message_input(const int& key) {
  if (key == 27) {
    mode = CHOICE;
    curs_set(0);
  } else if ((key == KEY_BACKSPACE || key == '\b') && input_pos > 0) {
    input_string.erase(input_string.begin() + input_pos - 1);
    input_pos--;
  } else if (key == '\n') {
    std::string message;
    if (input_string == "") return;
    if (current_channel >= 0)
      message = "m:" + std::to_string(current_channel) + ":" + input_string;
    else {
      message = "c:" + input_string;
      mode = CHANNELS;
    }
    send_message(message);
    input_string = "";
    input_pos = 0;
  } else if ((input_pos < right_width - 2) && (key >= 32 && key <= 126)) {
    if ((size_t) input_pos == input_string.size()) {
      input_string.push_back(key);
    } else {
      input_string.insert(input_string.begin()+input_pos, key);
    }
    input_pos++;
  } else if (input_pos > 0 && key == KEY_LEFT) {
    input_pos--;
  } else if ((size_t) input_pos < input_string.size() && key == KEY_RIGHT) {
    input_pos++;
  } else if (key == KEY_HOME) {
    input_pos = 0;
  } else if (key == KEY_END) {
    input_pos = input_string.size();
  }
}

void tt::chat::client::Client::take_chat_input(const int& key) {
  if (key == 27) {
    mode = CHOICE;
  } else if ((key == KEY_UP || key == 'k') && (size_t) scroll_offset < chats.size()-1) {
    scroll_offset++;
  } else if ((key == KEY_DOWN || key == 'j') && scroll_offset > 0) {
    scroll_offset--;
  }
}

void tt::chat::client::Client::take_channel_input(const int& key) {
  if (key == 27) {
    mode = CHOICE;
  } else if (key == KEY_UP && selected_channel > -1) {
    selected_channel--;
  } else if (key == KEY_DOWN && selected_channel < get_channel_count() - 1) {
    selected_channel++;
  } else if (key == '\n') {
    chats.clear();
    current_channel = selected_channel;
    mode = INPUT;
    if (current_channel > -1) {
      std::string message = "t:" + std::to_string(current_channel);
      send_message(message);
    }
  }
}

void receiver(tt::chat::client::Client* client) {
  client->receive_thread();
}

void tt::chat::client::Client::refresh_windows() {
  if (mode == INPUT) curs_set(1);
  else curs_set(0);

  werase(channel_win);
  box(channel_win, 0, 0);
  mvwprintw(channel_win, 0, 2, "%s", (mode == CHANNELS) ? " Channels [F] " : " Channels ");

  if (selected_channel == -1) {
    wattron(channel_win, A_REVERSE);
  }
  mvwprintw(channel_win, 1, 1, "New Channel");
  wattroff(channel_win, A_REVERSE);
  {
    std::lock_guard lock{channel_mutex};
    for (int i = 0; i < get_channel_count(); i++) {
      if (i == selected_channel) {
        wattron(channel_win, A_REVERSE);
      }
      mvwprintw(channel_win, i+2, 1, "%s", get_channel_by_id(i).c_str());
      wattroff(channel_win, A_REVERSE);
    }
  }

  // Draw chat box
  werase(chat_win);
  box(chat_win, 0, 0);
  if (current_channel >= 0)
    mvwprintw(chat_win, 0, 2, "%s", (std::string(" ") + get_channel_by_id(current_channel) + " " + ((mode == CHAT) ? "[F] " : "")).c_str());
  else
    mvwprintw(chat_win, 0, 2, "%s", (std::string(" ") + "New Channel" + " " + ((mode == CHAT) ? "[F] " : "")).c_str());
  
  {
    std::lock_guard lock{chat_mutex};
    int ind = right_height-2;
    for (int i = chats.size()-scroll_offset-1; i>=0 && ind >= 1; i--) {
      mvwprintw(chat_win, ind, 1, "%s", (chats[i].user + "\t:  " + chats[i].message).c_str());
      ind--;
    }
  }
  
  werase(help_win);
  std::string help;
  if (mode == CHOICE)
    help = CHOICE_HELP;
  else if (mode == CHANNELS)
    help = CHANNEL_HELP;
  else if (mode == CHAT)
    help = CHAT_HELP;
  else if (mode == INPUT)
    help = INPUT_HELP;
  mvwprintw(help_win, 0, 1, "%s", help.c_str());
  // clrtoeol();

  // Draw input
  werase(input_win);
  box(input_win, 0, 0);
  mvwprintw(input_win, 0, 2, " Input (%s mode) ", mode == INPUT ? "Insert" : "Nav");
  mvwprintw(input_win, 1, 1, "%s", input_string.c_str());
  if (mode == INPUT) {
    wmove(input_win, 1, 1 + input_pos);
  }

  
  refresh();
  
  // Refresh windows
  wrefresh(help_win);
  wrefresh(channel_win);
  wrefresh(chat_win);
  wrefresh(input_win);
}

void tt::chat::client::Client::ui_thread() {
  setlocale(LC_ALL, "");
  initscr();
  set_escdelay(25);
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0);

  getmaxyx(stdscr, height, width);

  height--;
  
  right_width = width - LEFT_WIDTH;
  right_height = height - INPUT_HEIGHT - 1;

  channel_win = newwin(height, LEFT_WIDTH, 0, 0);
  chat_win = newwin(right_height, right_width, 0, LEFT_WIDTH);
  input_win = newwin(INPUT_HEIGHT, right_width, right_height, LEFT_WIDTH);
  help_win = newwin(1, width, height, 0);

  std::thread receive(receiver, this);

  scroll_offset = 0;
  selected_channel = 0;
  mode = CHOICE;
  input_pos = 0;

  keypad(input_win, TRUE);

  int key = 27; // ESC
  do {

    if (mode == INPUT) curs_set(1);
    else curs_set(0);

    getmaxyx(stdscr, height, width);

    height--;

    right_width = width - LEFT_WIDTH;
    right_height = height - INPUT_HEIGHT;

    wresize(channel_win, height, LEFT_WIDTH);
    wresize(chat_win, right_height, right_width);
    wresize(input_win, INPUT_HEIGHT, right_width);
    mvwin(input_win, right_height, LEFT_WIDTH);

    if (mode == CHOICE) {
      take_choice_input(key);
    } else if (mode == CHAT) {
      take_chat_input(key);
    } else if (mode == INPUT) {
      take_message_input(key);
    } else if (mode == CHANNELS) {
      take_channel_input(key);
    }

    {
      std::lock_guard lock{refresh_mutex};
      refresh_windows();
    }

  } while (((key = getch()) != 'q') || (mode != CHOICE));

  running.store(false);
  send_message("k");

  receive.join();

  delwin(channel_win);
  delwin(chat_win);
  delwin(input_win);
  delwin(help_win);
  endwin();
}