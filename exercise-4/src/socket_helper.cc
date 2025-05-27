#include "socket_helper.h"

template <typename T, typename S> void check_error(T test, S error_message) {
  if (test) {
    std::cerr << error_message << "\n";
    exit(EXIT_FAILURE);
  }
}

int create_socket() {
  int my_sock;
  my_sock = socket(AF_INET, SOCK_STREAM, 0);
  check_error(my_sock < 0, "Socket creation error\n");
  return my_sock;
}