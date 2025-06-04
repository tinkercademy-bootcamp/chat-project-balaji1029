#pragma once

#include <ncurses.h>
#include <string>

namespace tt::chat::client{
  class Window {
    WINDOW* win;
  
    public:
    Window(int height, int width, int x, int y);
    void erase();
    void resize(int height, int width);
    void move(int x, int y);
    void box(int x, int y);
    void print(int x, int y, const std::string & message);
    void move_cursor(int x, int y);
    void refresh();
    ~Window();
  
  };
}
