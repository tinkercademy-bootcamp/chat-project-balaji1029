#pragma once

#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <netinet/in.h>
#include <ncurses.h>
#include <string>

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
    void receive_thread(WINDOW* input_win, WINDOW* chat_win, WINDOW* channel_win);
    
    void push_channel_name(const std::string &channel_name);
    int get_channel_count();
    std::string get_channel_by_id(const int &id);

    void take_choice_input(const int& key);
    void take_message_input(const int& key);
    
    std::vector<Message> chats;
    int selected_channel;
    int current_channel;
    Mode mode;
    int input_pos;
    std::string input_string;
    std::atomic<bool> running;      

private:
    int socket_;
    int right_width;
    std::vector<std::string> channel_names;
    
    std::mutex chat_mutex;          
    std::mutex channel_mutex;       
    
    // Helper methods
    sockaddr_in create_server_address(const std::string &server_ip, int port);
    void connect_to_server(int sock, sockaddr_in &server_address);

    static constexpr int kBufferSize = 1024;
};

} // namespace tt::chat::client