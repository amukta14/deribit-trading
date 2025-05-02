#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../api/deribit_api.hpp"

namespace goquant {

struct Order {
    std::string order_id;
    std::string instrument_name;
    double amount;
    double price;
    std::string type;
    std::string status;
    std::string side;
    std::chrono::system_clock::time_point timestamp;
};

class OrderManager {
public:
    OrderManager(std::shared_ptr<DeribitAPI> api);
    ~OrderManager();

    // Order operations
    Order place_order(
        const std::string& instrument_name,
        double amount,
        const std::string& type,
        double price = 0.0
    );

    bool cancel_order(const std::string& order_id);
    bool modify_order(
        const std::string& order_id,
        double new_amount,
        double new_price
    );

    // Order queries
    Order get_order(const std::string& order_id);
    std::vector<Order> get_open_orders();
    std::vector<Order> get_order_history();

    // Position management
    nlohmann::json get_positions();
    double get_position_size(const std::string& instrument_name);

private:
    std::shared_ptr<DeribitAPI> api;
    std::unordered_map<std::string, Order> active_orders;
    std::mutex orders_mutex;
};

} // namespace goquant 