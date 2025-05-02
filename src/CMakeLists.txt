add_library(goquant_core
    api/deribit_api.cpp
    websocket/websocket_client.cpp
    websocket/websocket_server.cpp
    order/order_manager.cpp
    market/market_data.cpp
    utils/logger.cpp
)

target_include_directories(goquant_core
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(goquant_core
    PUBLIC
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    Boost::system
) 