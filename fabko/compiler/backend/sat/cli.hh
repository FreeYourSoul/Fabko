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

#include "cli_base.hh"
#include "common/logging.hh"

#include <fil/cli/command_line_interface.hh>

namespace fabko::compiler::sat {

inline fil::sub_command make_cli() {
    fil::sub_command command_sat(
        "sat",
        [] { //
            log_info("execution of the SAT solver command line interface", "sat");
        },
        "SAT solver cli command");

    return command_sat;
}

} // namespace fabko::compiler::sat

#endif // CLI_HH
