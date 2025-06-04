#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <ncurses.h>
#include <thread>
#include <mutex>

#include <spdlog/spdlog.h>

#include "client/chat-client.h"
#include "../utils.h"

#define LEFT_WIDTH 35
#define INPUT_HEIGHT 3
#define MAX_LINES 1000

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

void receive_thread(tt::chat::client::Client* client, WINDOW* input_win, WINDOW* chat_win, WINDOW* channel_win) {
  client->receive_thread(input_win, chat_win, channel_win);
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

  std::string channels_str = client.receive_message();
  // std::cout << channel_num_str << std::endl;
  // client.send_message(channel_num_str);
  
  while (channels_str.find_first_of(';') != std::string::npos) {
    int semi_colon_index = channels_str.find_first_of(';');
    client.push_channel_name(channels_str.substr(0, semi_colon_index));
    channels_str = channels_str.substr(semi_colon_index + 1, channels_str.size()-semi_colon_index-1);
  }

  // for (int i=0; i<client.get_channel_count(); i++) {
  //   std::cout << i << ": " << client.get_channel_by_id(i) << std::endl;
  // }

  
  initscr();
  set_escdelay(25);
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
  
  std::thread receive(receive_thread, &client, input_win, chat_win, channel_win);

  // std::string client.input_string;
  int scroll_offset = 0;

  keypad(input_win, TRUE);

  int key = 27; // ESC
  do {

    getmaxyx(stdscr, height, width);
    right_width = width - LEFT_WIDTH;
    right_height = height - INPUT_HEIGHT;

    wresize(channel_win, height, LEFT_WIDTH);
    wresize(chat_win, right_height, right_width);
    wresize(input_win, INPUT_HEIGHT, right_width);
    mvwin(input_win, right_height, LEFT_WIDTH);

    if (client.mode == CHOICE) {
      if (key == 'w') {
        client.mode = CHANNELS;
      } else if (key == 'c') {
        client.mode = CHAT;
      } else if (key == 'i') {
        client.mode = INPUT;
        curs_set(1);
      } else if (key == 'q') {
        client.running.store(false);
        client.send_message("k");
        break;
      }
    } else if (client.mode == CHAT) {
      if (key == 27) {
        client.mode = CHOICE;
      } else if ((key == KEY_UP || key == 'k') && scroll_offset < 100) {
        scroll_offset++;
      } else if ((key == KEY_DOWN || key == 'j') && scroll_offset > 0) {
        scroll_offset--;
      } 
    } else if (client.mode == INPUT) {
      if (key == 27) {
        client.mode = CHOICE;
        curs_set(0);
      } else if ((key == KEY_BACKSPACE || key == '\b') && client.input_pos > 0) {
        client.input_string.erase(client.input_string.begin() + client.input_pos - 1);
        client.input_pos--;
      } else if (key == '\n') {
        std::string message;
        if (client.current_channel >= 0)
          message = "m:" + std::to_string(client.current_channel) + ":" + client.input_string;
        else 
          message = "c:" + client.input_string;
        client.send_message(message);
        client.input_string = "";
        client.input_pos = 0;
      } else if ((client.input_pos < right_width - 2) && (key >= 32 && key <= 126)) {
        if (client.input_pos == client.input_string.size()) {
          client.input_string.push_back(key);
        } else {
          client.input_string.insert(client.input_string.begin()+client.input_pos, key);
        }
        client.input_pos++;
      } else if (client.input_pos > 0 && key == KEY_LEFT) {
        client.input_pos--;
      } else if (client.input_pos < client.input_string.size() && key == KEY_RIGHT) {
        client.input_pos++;
      } else if (key == KEY_HOME) {
        client.input_pos = 0;
      } else if (key == KEY_END) {
        client.input_pos = client.input_string.size();
      }
    } else if (client.mode == CHANNELS) {
      if (key == 27) {
        client.mode = CHOICE;
      } else if (key == KEY_UP && client.selected_channel > -1) {
        client.selected_channel--;
      } else if (key == KEY_DOWN && client.selected_channel < client.get_channel_count() - 1) {
        client.selected_channel++;
      } else if (key == '\n') {
        client.chats.clear();
        client.current_channel = client.selected_channel;
        client.mode = INPUT;
        std::string message = "t:" + std::to_string(client.current_channel);
        client.send_message(message);
      }
    }

    // Draw channel box
		werase(channel_win);
		box(channel_win, 0, 0);
		mvwprintw(channel_win, 0, 2, (client.mode == CHANNELS) ? " Channels [F] " : " Channels ");

    if (client.selected_channel == -1) {
      wattron(channel_win, A_REVERSE);
    }
    mvwprintw(channel_win, 1, 1, "New Channel");
    wattroff(channel_win, A_REVERSE);
    for (int i = 0; i < client.get_channel_count(); i++) {
      if (i == client.selected_channel) {
        wattron(channel_win, A_REVERSE);
      }
      mvwprintw(channel_win, i+2, 1, "%s", client.get_channel_by_id(i).c_str());
      wattroff(channel_win, A_REVERSE);
    }

    // Draw chat box
		werase(chat_win);
		box(chat_win, 0, 0);
		mvwprintw(chat_win, 0, 2, (std::string(" ") + client.get_channel_by_id(client.current_channel) + " " + ((client.mode == CHAT) ? "[F] " : "")).c_str());
    // mvwprintw(chat_win, 1, 1, "Key: %d", key);
    for (int i=0; i<client.chats.size(); i++) {
      mvwprintw(chat_win, i+1, 1, "%s", (client.chats[i].user + "\t:  " + client.chats[i].message).c_str());
    }

    // Draw input
		werase(input_win);
		box(input_win, 0, 0);
		mvwprintw(input_win, 0, 2, " Input (%s mode) ", client.mode == INPUT ? "Insert" : "Nav");
		mvwprintw(input_win, 1, 1, "%s", client.input_string.c_str());
		if (client.mode == INPUT) {
			wmove(input_win, 1, 1 + client.input_pos);
		}

    // Refresh windows
		wrefresh(channel_win);
		wrefresh(chat_win);
		wrefresh(input_win);

  } while (((key = getch()) != 'q') || (client.mode != CHOICE));



  receive.join();

  delwin(channel_win);
  delwin(chat_win);
  delwin(input_win);
  endwin();

  // client.send_and_receive_message("u:disco")

  // while (true) {
  //   // std::cout << "Enter the message: ";
  //   std::cin >> message;
  //   response = client.send_and_receive_message(message);
  //   SPDLOG_INFO("Received back: {}", response);
  // }

  return 0;
}