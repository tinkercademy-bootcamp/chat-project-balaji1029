#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include <ncurses.h>

#include <spdlog/spdlog.h>

#include "client/chat-client.h"
#include "../utils.h"

#define LEFT_WIDTH 35
#define INPUT_HEIGHT 3
#define MAX_LINES 1000

enum Mode {
  CHOICE,
  CHANNELS,
  CHAT,
  INPUT,
};

std::string read_args(int argc, char *argv[]) {
  using namespace tt::chat;
  std::string message = "Hello from client";
  if (argc == 1) {
    std::cout << "Usage: " << argv[0] << " <username>\n";
    exit(EXIT_FAILURE);
  }
  if (argc > 1) {
    message = argv[1];
  }
  return message;
}

int main(int argc, char *argv[]) {
  const int kPort = 8080;
  const std::string kServerAddress = "127.0.0.1";

  std::string message = read_args(argc, argv);

  tt::chat::client::Client client{kPort, kServerAddress};
  std::string response = client.send_and_receive_message(message);

  if (response == "unavailable") {
    SPDLOG_ERROR("Username {} taken", message);
    return 1;
  }

  std::string channel_num_str = client.receive_message();
  
  for (int i=0; i<std::stoi(channel_num_str); i++) {
    client.push_channel_name(client.receive_message());
  }

  for (int i=0; i<client.get_channel_count(); i++) {
    std::cout << i << ": " << client.get_channel_by_id(i) << std::endl;
  }

  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0);

  int height, width;
  getmaxyx(stdscr, height, width);

  int right_width = width - LEFT_WIDTH;
  int right_height = width - INPUT_HEIGHT;

  WINDOW* channel_win = newwin(height, LEFT_WIDTH, 0, 0);
  WINDOW* chat_win = newwin(right_height, right_width, 0, LEFT_WIDTH);
  WINDOW* input_win = newwin(INPUT_HEIGHT, right_width, right_height, LEFT_WIDTH);

  Mode mode = CHOICE;
  char input[256] = "";
  int input_pos = 0;

  char key = 27; // ESC

  delwin(channel_win);
  delwin(chat_win);
  delwin(input_win);

  while (true) {
    std::cout << "Enter the message: ";
    std::cin >> message;
    response = client.send_and_receive_message(message);
    SPDLOG_INFO("Received back: {}", response);
  }

  return 0;
}
