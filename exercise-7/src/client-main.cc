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

#include <spdlog/spdlog.h>

#include "client/chat-client.h"
#include "../utils.h"

namespace {
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
} // namespace

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

  while (true) {
    std::cout << "Enter the message: ";
    std::cin >> message;
    response = client.send_and_receive_message(message);
    SPDLOG_INFO("Received back: {}", response);
  }

  return 0;
}
