#ifndef SOCKET_HELPER_H
#define SOCKET_HELPER_H

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

template <typename T, typename S> void check_error(T test, S error_message);

int create_socket();

#endif // SOCKET_HELPER_H