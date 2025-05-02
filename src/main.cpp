#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <csignal>

#include "api/deribit_api.hpp"
#include "websocket/websocket_server.hpp"
#include "order/order_manager.hpp"
#include "utils/logger.hpp"

using namespace goquant;

std::atomic<bool> running(true);

void signal_handler(int signal) {
    running = false;
}

int main(int argc, char* argv[]) {
    // Set up signal handling
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Initialize logger
    Logger::init("goquant.log");

    try {
        // Get API credentials from environment variables
        const char* api_key = std::getenv("DERIBIT_API_KEY");
        const char* api_secret = std::getenv("DERIBIT_API_SECRET");

        if (!api_key || !api_secret) {
            std::cerr << "Error: Please set DERIBIT_API_KEY and DERIBIT_API_SECRET environment variables" << std::endl;
            return 1;
        }

        // Initialize components
        auto api = std::make_shared<DeribitAPI>(api_key, api_secret);
        auto order_manager = std::make_unique<OrderManager>(api);
        auto ws_server = std::make_unique<WebSocketServer>(8080);

        // Authenticate with Deribit
        if (!api->authenticate()) {
            std::cerr << "Failed to authenticate with Deribit" << std::endl;
            return 1;
        }

        // Start WebSocket server
        if (!ws_server->start()) {
            std::cerr << "Failed to start WebSocket server" << std::endl;
            return 1;
        }

        // Set up WebSocket callbacks
        ws_server->set_message_callback([](const std::string& msg) {
            Logger::info("Received message: " + msg);
        });

        ws_server->set_connection_callback([](int client_id) {
            Logger::info("New client connected: " + std::to_string(client_id));
        });

        // Main loop
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Cleanup
        ws_server->stop();
        Logger::info("Application shutdown complete");

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 