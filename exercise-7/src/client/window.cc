#include "window.h"

tt::chat::client::Window::Window(int height, int width, int x, int y) {
  newwin(height, width, x, y);
}

void tt::chat::client::Window::key(bool is_keypad) {
  keypad(win, is_keypad);
}

void tt::chat::client::Window::erase() {
  werase(win);
}

void tt::chat::client::Window::resize(int height, int width) {
  wresize(win, height, width);
}

void tt::chat::client::Window::move(int x, int y) {
  wresize(win, x, y);
}

void tt::chat::client::Window::boxit(int x, int y) {
  box(win, x, y);
}

void tt::chat::client::Window::wattr(bool on) {
  if (on) wattron(win, A_REVERSE);
  else wattroff(win, A_REVERSE);
}

void tt::chat::client::Window::print(int x, int y, const std::string & message) {
  mvwprintw(win, x, y, message.c_str());
}

void tt::chat::client::Window::move_cursor(int x, int y) {
  wmove(win, x, y);
}

void tt::chat::client::Window::refresh() {
  wrefresh(win);
}

tt::chat::client::Window::~Window() {
  delwin(win);
}