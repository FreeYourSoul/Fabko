// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <fstream>

#include <common/exception.hh>
#include <common/logging.hh>
#include <logic/sat/solver.hh>

#include "common/string_utils.hh"
#include "dimacs_compiler.hh"

namespace {

inline std::pair<std::string_view, std::string_view> next_token(std::string_view buffer) {
    const auto it_find = std::find_if(buffer.begin(), buffer.end(), [](char c) { return std::isspace(c) || c == '\n'; });
    return {
        {buffer.begin(), it_find     },
        fabko::trim_string({it_find,        buffer.end()}
        )
    };
}

inline std::vector<fabko::sat::literal> parse_clause(std::size_t line_number, long nb_variable, std::string& line) {
    std::string token;
    std::vector<fabko::sat::literal> literals;

    while (!line.empty()) {
        std::tie(token, line) = next_token(line);

        if (const auto value = std::stol(std::string(token.data(), token.size())); value != 0) {
            auto v = std::abs(value);

            fabko::fabko_assert(
                v <= nb_variable,
                fmt::format("error line {}: {} cannot be set. Variable {}", line_number, v, nb_variable));

            literals.emplace_back(static_cast<fabko::sat::variable>(v), value > 0);
        }
    }
    return literals;
}

inline std::pair<long, long> parse_dimacs_header(std::string& line) {
    std::string token;

    std::tie(token, line) = next_token(line);
    fabko::fabko_assert(token == "p", "unrecognised format :: problem definition should start with 'p'");

    std::tie(token, line) = next_token(line);
    fabko::fabko_assert(token == "cnf", "dimacs compiler support only cnf problems :: unrecognised format");

    std::tie(token, line)        = next_token(line);
    const auto nb_variable_token = std::stol(std::string(token.data(), token.size()));

    std::tie(token, line)      = next_token(line);
    const auto nb_clause_token = std::stol(std::string(token.data(), token.size()));

    return {nb_variable_token, nb_clause_token};
}

} // namespace

namespace fabko::sat {

solver from_dimacs(const std::string& file_path) {
    log_info("parse dimacs file {} into sat::solver", file_path);

    std::ifstream infile(file_path);
    fabko_assert(bool(infile), "File doesn't exists :: an existing path should be provided to the function `from_dimacs`");

    std::optional<std::pair<long, long>> dimacs_header{};
    std::string line;
    std::size_t line_number = 0;

    auto s = solver{solver_config{}};

    while (std::getline(infile, line) && !trim_string(line).starts_with("c")) {
        log_trace("line {}", line);

        if (dimacs_header.has_value()) {
            s.add_clause(parse_clause(line_number, dimacs_header.value().first, line));
        } else {
            dimacs_header = parse_dimacs_header(line);
            s.add_variables(static_cast<size_t>(dimacs_header.value().first));
        }
        ++line_number;
    }

    // validate creation
    fabko_assert(
        s.nb_variables(),
        fmt::format("number of variable set are wrong :: expected {} :: currently set {}",
                    dimacs_header.value().first, s.nb_variables()));
    fabko_assert(
        s.nb_clauses(),
        fmt::format("number of clauses set are wrong :: expected {} :: currently set {}",
                    dimacs_header.value().second, s.nb_clauses()));

    log_info("end of parsing file {}", file_path);

    return s;
}

} // namespace fabko::sat
