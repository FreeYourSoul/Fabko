// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 30/04/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <string>

#include <spdlog/spdlog.h>

namespace fabko {

namespace logging_details {
constexpr auto global_logger_name = "fabko";

spdlog::level::level_enum get_env_log();

}

/**
 * Initialize the logger of fabko.
 *
 * @note Should be called before any usage of Fabko in order to ensure a proper logging of the
 * library when functioning.
 *
 * Some component of the fabko library initialize the logger automatically. It is however recommended
 * to make an explicit call to init_logger in order to set a logging file.
 * Only the log level can be set by environment variable.
 *
 * The component initializing the logger if not already done are the following :
 *   - SAT solver (instantiation)
 *
 * @param level logging level at which the application is set
 * @param log_file file into which the log should be stored
 */
void init_logger(spdlog::level::level_enum level=logging_details::get_env_log(), const std::string& log_file = "");

/**
 * log an informative message
 *
 * @param log message to log
 * @param packs parameter pack in case of formatting usage via fmt library
 */
template<typename... Args>
void log_info(spdlog::format_string_t<Args...> log, Args... packs) {
  spdlog::get(logging_details::global_logger_name)->info(std::move(log), std::forward<Args>(packs)...);

}

/**
 * log a trace message
 *
 * @param log message to log
 * @param packs parameter pack in case of formatting usage via fmt library
 */
template<typename... Args>
void log_trace(spdlog::format_string_t<Args...> log, Args... packs) {
  spdlog::get(logging_details::global_logger_name)->trace(std::move(log), std::forward<Args>(packs)...);
}

/**
 * log a debug message
 *
 * @param log message to log
 * @param packs parameter pack in case of formatting usage via fmt library
 */
template<typename... Args>
void log_debug(spdlog::format_string_t<Args...> log, Args... packs) {
  spdlog::get(logging_details::global_logger_name)->debug(std::move(log), std::forward<Args>(packs)...);
}

/**
 * log a warning
 *
 * @param log message to log
 * @param packs parameter pack in case of formatting usage via fmt library
 */
template<typename... Args>
void log_warn(spdlog::format_string_t<Args...> log, Args... packs) {
  spdlog::get(logging_details::global_logger_name)->warn(std::move(log), std::forward<Args>(packs)...);
}

/**
 * log an error
 *
 * @param log message to log
 * @param packs parameter pack in case of formatting usage via fmt library
 */
template<typename... Args>
void log_error(spdlog::format_string_t<Args...> log, Args... packs) {
  spdlog::get(logging_details::global_logger_name)->error(std::move(log), std::forward<Args>(packs)...);
}

}// namespace fabko
