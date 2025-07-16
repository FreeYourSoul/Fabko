// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 26.06.25. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#ifndef CLI_BASE_HH
#define CLI_BASE_HH

#include <spdlog/spdlog.h>

namespace fabko {

struct cli_data {

    spdlog::level::level_enum log_level = spdlog::level::debug; //!< log level to use for the CLI, can be set by the environment variable FABKO_LOG_LEVEL
    std::string log_file                = "/tmp/fabko.log";     //!< log file to use for the CLI, if empty no log file is used
};

} // namespace fabko

#endif // CLI_BASE_HH
