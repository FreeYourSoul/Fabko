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

#ifndef CLI_HH
#define CLI_HH

#include <fil/cli/command_line_interface.hh>
#include <filesystem>
#include <fmt/format.h>

#include "common/logging.hh"
#include "compiler/backend/sat/solver.hh"

namespace fabko::compiler::sat {

inline fil::sub_command make_cli() {

    auto files = std::make_shared<std::vector<std::filesystem::path>>();

    fil::sub_command command_sat(
        "sat",
        [files] { //
            log_info("execution of the SAT solver command line interface");
            if (files->empty()) {
                log_error("No file provided to the SAT solver, please use --file or -f option to provide a file");
                return;
            }
            log_info("file to process count : {}", files->size());
            for (const auto& cnf_file : *files) {
                log_info("Processing file: {}", cnf_file.string());
                auto model = make_model_from_cnf_file(cnf_file);
                solver solver {std::move(model)};
                auto results = solver.solve(1);
            }
        },
        "SAT solver cli command");
    command_sat.add_option(fil::option {    //
        "--cnf-file",
        "-c",
        [files](const std::string& value) { //
            std::filesystem::path dnf_file {value};
            if (!exists(dnf_file) || !dnf_file.has_extension() || dnf_file.extension() != ".cnf") {
                log_error("file {} is not a valid CNF file, it should exists and have a .cnf extension", std::string(dnf_file));
                return;
            }
            files->emplace_back(std::move(dnf_file));
        },
        "File in CNF format to be process by the SAT solver"});

    return command_sat;
}

} // namespace fabko::compiler::sat
#endif // CLI_HH
