// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 30/04/23. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <optional>
#include <string>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "logging.hh"

namespace fabko {

constexpr auto logger_pattern = "[%Y-%m-%d %H:%M:%S:%e][fabko][%=7l%$]: %v";

spdlog::level::level_enum logging_details::get_env_log() {
    // retrieve
    return spdlog::level::trace;
}

void init_logger(spdlog::level::level_enum level, const std::string& log_file) {

    if (spdlog::get(logging_details::global_logger_name) != nullptr) {
        return;
    }

    std::vector<spdlog::sink_ptr> sinks {std::make_shared<spdlog::sinks::stdout_color_sink_mt>()};

    if (!log_file.empty()) {
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true));
    }
    auto logger = std::make_shared<spdlog::logger>(logging_details::global_logger_name, sinks.begin(), sinks.end());
    logger->set_pattern(logger_pattern);
    logger->set_level(level);
    spdlog::register_logger(logger);
}

} // namespace fabko