#include "socket_helper.h"

int create_socket() {
  int my_sock;
  my_sock = socket(AF_INET, SOCK_STREAM, 0);
  check_error(my_sock < 0, "Socket creation error\n");
  return my_sock;
}