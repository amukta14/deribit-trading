#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace goquant {

class Logger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static void init(const std::string& filename);
    static void set_level(Level level);
    
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);

private:
    static Logger& instance();
    void log(Level level, const std::string& message);
    std::string level_to_string(Level level);
    std::string get_timestamp();

    std::ofstream log_file;
    Level current_level;
    std::mutex log_mutex;
    bool initialized;
};

} // namespace goquant 