#pragma once

#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <netinet/in.h>
#include <ncurses.h>
#include <string>

#include "window.h"

#define SERVER_ERROR "Server closed connection.\n"
#define READ_ERROR "Read error.\n"

struct Message {
    std::string user;
    std::string message;
};

enum Mode {
  CHOICE,
  CHANNELS,
  CHAT,
  INPUT,
};

namespace tt::chat::client {

class Client {
public:
    Client(int port, const std::string &server_address);
    ~Client();
    
    std::string send_and_receive_message(const std::string &message);
    int send_message(const std::string& message);
    std::string receive_message();
    void receive_thread(tt::chat::client::Window &input_win, tt::chat::client::Window &chat_win, tt::chat::client::Window &channel_win);
    
    void push_channel_name(const std::string &channel_name);
    int get_channel_count();
    std::string get_channel_by_id(const int &id);
    
    std::vector<Message> chats;
    std::atomic<size_t> selected_channel;
    std::atomic<size_t> current_channel;
    std::atomic<Mode> mode;
    std::atomic<size_t> input_pos;
    std::string input_string;
    std::atomic<bool> running;      

private:
    int socket_;
    std::vector<std::string> channel_names;
    
    std::mutex chat_mutex;          
    std::mutex channel_mutex;       
    
    // Helper methods
    sockaddr_in create_server_address(const std::string &server_ip, int port);
    void connect_to_server(int sock, sockaddr_in &server_address);

    static constexpr int kBufferSize = 1024;
};

} // namespace tt::chat::client