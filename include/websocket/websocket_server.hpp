#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>

namespace goquant {

class WebSocketServer {
public:
    using MessageCallback = std::function<void(const std::string&)>;
    using ConnectionCallback = std::function<void(int)>;

    WebSocketServer(int port = 8080);
    ~WebSocketServer();

    // Server control
    bool start();
    void stop();

    // Client management
    void broadcast(const std::string& message);
    void send_to_client(int client_id, const std::string& message);

    // Subscription management
    void subscribe(int client_id, const std::string& symbol);
    void unsubscribe(int client_id, const std::string& symbol);

    // Callback registration
    void set_message_callback(MessageCallback callback);
    void set_connection_callback(ConnectionCallback callback);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
    std::atomic<bool> running;
    std::thread server_thread;
};

} // namespace goquant 