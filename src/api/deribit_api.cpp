#include "api/deribit_api.hpp"
#include "utils/logger.hpp"
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace goquant {

class DeribitAPI::Impl {
public:
    Impl(const std::string& api_key, const std::string& api_secret)
        : api_key(api_key), api_secret(api_secret), base_url("https://test.deribit.com/api/v2/") {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~Impl() {
        curl_global_cleanup();
    }

    bool authenticate() {
        try {
            auto response = send_request("public/auth", {
                {"grant_type", "client_credentials"},
                {"client_id", api_key},
                {"client_secret", api_secret}
            });
            
            return response.contains("result") && response["result"].contains("access_token");
        } catch (const std::exception& e) {
            Logger::error("Authentication failed: " + std::string(e.what()));
            return false;
        }
    }

    nlohmann::json place_order(
        const std::string& instrument_name,
        double amount,
        const std::string& type,
        double price
    ) {
        nlohmann::json params = {
            {"instrument_name", instrument_name},
            {"amount", amount},
            {"type", type}
        };

        if (price > 0) {
            params["price"] = price;
        }

        return send_request("private/buy", params);
    }

    nlohmann::json cancel_order(const std::string& order_id) {
        return send_request("private/cancel", {
            {"order_id", order_id}
        });
    }

    nlohmann::json modify_order(
        const std::string& order_id,
        double new_amount,
        double new_price
    ) {
        return send_request("private/edit", {
            {"order_id", order_id},
            {"amount", new_amount},
            {"price", new_price}
        });
    }

    nlohmann::json get_orderbook(const std::string& instrument_name) {
        return send_request("public/get_order_book", {
            {"instrument_name", instrument_name}
        });
    }

    nlohmann::json get_positions() {
        return send_request("private/get_positions", {});
    }

    nlohmann::json get_instruments(const std::string& currency, const std::string& kind) {
        return send_request("public/get_instruments", {
            {"currency", currency},
            {"kind", kind}
        });
    }

private:
    std::string api_key;
    std::string api_secret;
    std::string base_url;
    std::string access_token;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    std::string generate_signature(const std::string& data) {
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hash_len;

        HMAC_CTX* hmac = HMAC_CTX_new();
        HMAC_Init_ex(hmac, api_secret.c_str(), api_secret.length(), EVP_sha256(), nullptr);
        HMAC_Update(hmac, (unsigned char*)data.c_str(), data.length());
        HMAC_Final(hmac, hash, &hash_len);
        HMAC_CTX_free(hmac);

        std::stringstream ss;
        for(unsigned int i = 0; i < hash_len; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    nlohmann::json send_request(const std::string& endpoint, const nlohmann::json& params) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        std::string url = base_url + endpoint;
        std::string response_string;
        std::string post_data = params.dump();

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        if (!access_token.empty()) {
            std::string auth_header = "Authorization: Bearer " + access_token;
            headers = curl_slist_append(headers, auth_header.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
        }

        return nlohmann::json::parse(response_string);
    }
};

DeribitAPI::DeribitAPI(const std::string& api_key, const std::string& api_secret)
    : pimpl(std::make_unique<Impl>(api_key, api_secret)) {}

DeribitAPI::~DeribitAPI() = default;

bool DeribitAPI::authenticate() {
    return pimpl->authenticate();
}

nlohmann::json DeribitAPI::place_order(
    const std::string& instrument_name,
    double amount,
    const std::string& type,
    double price
) {
    return pimpl->place_order(instrument_name, amount, type, price);
}

nlohmann::json DeribitAPI::cancel_order(const std::string& order_id) {
    return pimpl->cancel_order(order_id);
}

nlohmann::json DeribitAPI::modify_order(
    const std::string& order_id,
    double new_amount,
    double new_price
) {
    return pimpl->modify_order(order_id, new_amount, new_price);
}

nlohmann::json DeribitAPI::get_orderbook(const std::string& instrument_name) {
    return pimpl->get_orderbook(instrument_name);
}

nlohmann::json DeribitAPI::get_positions() {
    return pimpl->get_positions();
}

nlohmann::json DeribitAPI::get_instruments(const std::string& currency, const std::string& kind) {
    return pimpl->get_instruments(currency, kind);
}

void DeribitAPI::subscribe_to_orderbook(const std::string& instrument_name) {
    // WebSocket subscription will be implemented separately
    Logger::info("Subscribing to orderbook for " + instrument_name);
}

void DeribitAPI::unsubscribe_from_orderbook(const std::string& instrument_name) {
    // WebSocket unsubscription will be implemented separately
    Logger::info("Unsubscribing from orderbook for " + instrument_name);
}

} // namespace goquant 