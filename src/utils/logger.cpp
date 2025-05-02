#include "utils/logger.hpp"
#include <iostream>

namespace goquant {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::init(const std::string& filename) {
    auto& inst = instance();
    std::lock_guard<std::mutex> lock(inst.log_mutex);
    inst.log_file.open(filename, std::ios::app);
    inst.initialized = true;
    inst.current_level = Level::INFO;
}

void Logger::set_level(Level level) {
    instance().current_level = level;
}

void Logger::debug(const std::string& message) {
    instance().log(Level::DEBUG, message);
}

void Logger::info(const std::string& message) {
    instance().log(Level::INFO, message);
}

void Logger::warning(const std::string& message) {
    instance().log(Level::WARNING, message);
}

void Logger::error(const std::string& message) {
    instance().log(Level::ERROR, message);
}

void Logger::log(Level level, const std::string& message) {
    if (!initialized || level < current_level) return;

    std::lock_guard<std::mutex> lock(log_mutex);
    std::string timestamp = get_timestamp();
    std::string level_str = level_to_string(level);
    
    std::string log_entry = timestamp + " [" + level_str + "] " + message + "\n";
    log_file << log_entry;
    log_file.flush();
    
    // Also output to console
    std::cout << log_entry;
}

std::string Logger::level_to_string(Level level) {
    switch (level) {
        case Level::DEBUG:   return "DEBUG";
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARNING";
        case Level::ERROR:   return "ERROR";
        default:            return "UNKNOWN";
    }
}

std::string Logger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

} // namespace goquant 