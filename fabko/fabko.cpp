// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <fil/cli/command_line_interface.hh>
#include <filesystem>

#include "cli_base.hh"
#include "common/logging.hh"
#include "compiler/backend/sat/cli.hh"

int main(int argc, char** argv) {

    fabko::cli_data cli_data;
    auto command_sat = fabko::compiler::sat::make_cli();

    fil::command_line_interface cli({command_sat}, [] {});
    cli.add_option(fil::option {//
        "--log-level",
        "-l",
        [&cli_data](const std::string& value) { cli_data.log_level = spdlog::level::from_str(value); },
        "Set the log level for the CLI, if not provided, the default log level is `info`.\n"
        "The possible values are the following (case insensitive):\n"
        " debug : maximum log level possible\n"
        " info  : all logs excluding debug logs are displayed\n"
        " warn  : only warning and error logs are displayed\n"
        " error : only error logs are displayed\n"});
    cli.add_option(fil::option {//
        "--log-file",
        [&cli_data](const std::string& value) { cli_data.log_file = value; },
        "Set the output log file in which logs will be written, if not provided : /tmp/fabko.log will be used by default."});

    cli.add_pre_executed_handler([&cli_data] { //
        fmt::println("Initialization of the logger");
        fabko::init_logger(cli_data.log_level, cli_data.log_file);
    });

    if (!cli.parse_command_line(argc, argv)) {
        fabko::log_error("Command line error occurred");
        return 1;
    }

    return 0;
}
