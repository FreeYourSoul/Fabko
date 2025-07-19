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

#include <algorithm>
#include <expected>
#include <filesystem>
#include <fstream>
#include <set>
#include <stdexcept>

#include "common/logging.hh"

#include "solver.hh"

namespace fabko::compiler::sat {

namespace impl_details {
std::expected<solver::result, sat_error> solve_sat(Solver_Context& ctx, const Model& model);
} // namespace impl_details

std::string to_string(assignment a) {
    switch (a) {
        case assignment::on: return "on";
        case assignment::off: return "off";
        default: return "unassigned";
    }
}

Model make_model_from_cnf_file(const std::filesystem::path& cnf_file) {
    if (!std::filesystem::exists(cnf_file)) {
        throw std::runtime_error("CNF file does not exist");
    }

    std::ifstream file(cnf_file);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open CNF file");
    }

    std::string line;
    std::vector<std::vector<Literal>> clauses;
    std::vector<Literal> literals;
    std::set<Literal> literals_unique;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == 'c') {
            continue; // Skip comments and empty lines
        }

        if (line[0] == 'p') {
            if (clauses.capacity() > 0) {
                throw std::runtime_error("Unexpected 'p' is specified twice");
            }

            std::istringstream iss(line);
            std::string p, cnf;
            std::size_t num_variables;
            std::size_t num_clauses;

            iss >> p >> cnf >> num_variables >> num_clauses;
            if (p != "p" || cnf != "cnf") {
                throw std::runtime_error("Invalid CNF format");
            }
            literals.reserve(num_variables);
            clauses.reserve(num_clauses);
            continue;
        }

        std::istringstream iss(line);
        std::vector<Literal> clause;
        std::int64_t lit;
        while (iss >> lit && lit != 0) {
            clause.emplace_back(lit);
            literals_unique.insert(Literal {std::abs(lit)});
        }

        if (!clause.empty()) {
            clauses.push_back(std::move(clause));
        }
    }

    fabko_assert(literals_unique.size() == literals.capacity(), //
        fmt::format("More literals than expected, expected {} but got {}", literals.capacity(), literals_unique.size()));

    fabko_assert(clauses.size() == clauses.capacity(),          //
        fmt::format("More clauses than expected, expected {} but got {}", clauses.capacity(), clauses.size()));

    std::ranges::transform(literals_unique, std::back_inserter(literals), [](const Literal& l) { return l; });

    return Model {std::move(literals), std::move(clauses)};
}

solver::solver(Model m)
    : context_(m)
    , model_(std::move(m)) {}

std::vector<solver::result> solver::solve(std::int32_t expected) {
    std::vector<result> res;

    for (auto i = 0; i < expected; ++i) {
        auto r = impl_details::solve_sat(context_, model_);
        if (!r.has_value()) {
            auto error = r.error();
            if (error == sat_error::unsatisfiable) {
                log_error("SAT solver cannot find solution for mode : UNSATISFIABLE");
            } else {
                log_error("SAT solver : an error occurred");
            }
            return res;
        }
        res.push_back(std::move(r.value()));

        // add a constraint to disable the found solution
    }

    return res;
}

} // namespace fabko::compiler::sat