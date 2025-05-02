#pragma once

#include <string>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>

namespace goquant {

class DeribitAPI {
public:
    DeribitAPI(const std::string& api_key, const std::string& api_secret);
    ~DeribitAPI();

    // Authentication
    bool authenticate();

    // Order Management
    nlohmann::json place_order(
        const std::string& instrument_name,
        double amount,
        const std::string& type,
        double price = 0.0
    );

    nlohmann::json cancel_order(const std::string& order_id);
    nlohmann::json modify_order(
        const std::string& order_id,
        double new_amount,
        double new_price
    );

    // Market Data
    nlohmann::json get_orderbook(const std::string& instrument_name);
    nlohmann::json get_positions();
    nlohmann::json get_instruments(const std::string& currency, const std::string& kind);

    // WebSocket
    void subscribe_to_orderbook(const std::string& instrument_name);
    void unsubscribe_from_orderbook(const std::string& instrument_name);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace goquant 