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

Solver_Context::Solver_Context(const Model& model)
    : config_(model.conf)
    , model_(model)
    , vars_soa_([&]() {
        Vars_Soa vars;
        vars.reserve(model.literals.size());
        for (const auto& lit : model.literals) {
            [[maybe_unused]] auto _ =
                vars.insert(lit, assignment::not_assigned, Assignment_Context {/*empty assignment context*/}, Metadata {/*@todo: add compiler context from model*/});
        }
        return vars;
    }())
    , clauses_soa_([&]() {
        Clauses_Soa clauses;
        clauses.reserve(model.clauses.size());

        for (const std::vector<Literal>& model_clause : model.clauses) {
            auto all_clause_ids = std::ranges::fold_left(model_clause, std::vector<Vars_Soa::struct_id> {}, [&, this](auto res, const Literal& l) { //
                auto it = std::ranges::find_if(vars_soa_, fil::soa_select<soa_literal>([&, this](Literal lit) {                                     //
                    return lit == l;
                }));
                fabko_assert(it != vars_soa_.end(), "a clause cannot contains a non-defined literal");
                ++get<soa_assignment_ctx>(*it).vsids_activity_;
                res.push_back((*it).struct_id());
                return res;
            });

            Clause clause_to_insert {model_clause, std::move(all_clause_ids)};
            [[maybe_unused]] const auto _ = clauses.insert( //
                clause_to_insert,
                Clause_Watcher {vars_soa_, clause_to_insert},
                Metadata {/*@todo: add compiler context from model*/});
        }
        return clauses;
    }()) {}

std::string to_string(assignment a) {
    switch (a) {
        case assignment::on: return "on";
        case assignment::off: return "off";
        default: return "unassigned";
    }
}

Clause_Watcher::Clause_Watcher(const Vars_Soa& vs, const Clause& clause)
    : watchers_([&]() -> std::array<std::optional<Vars_Soa::struct_id>, 2> { //
        fabko_assert(!clause.get_literals().empty(), "Cannot make a clause watchers over an empty clause");

        auto filtered =
            std::ranges::views::filter(clause.get_literals(), [&vs](const auto& lit_id_pair) { return get<soa_assignment>(vs[lit_id_pair.second]) == assignment::not_assigned; }) //
            | std::views::transform([&vs](const auto& lit_id_pair) { return vs[lit_id_pair.second].struct_id(); });                                                               //

        auto it = filtered.begin();
        if (it == filtered.end())
            return {std::nullopt, std::nullopt};      // no watched literals, clause is satisfied
        if (++it == filtered.end())
            return {*filtered.begin(), std::nullopt}; // no watched literals, clause is satisfied

        return {std::make_optional(*filtered.begin()), std::make_optional(*it)};
    }()) {}

void Clause_Watcher::replace(const Vars_Soa& vs, const Clause& clause, Vars_Soa::struct_id to_replace) {

    if ((!watchers_[0].has_value() || watchers_[0].value().offset != to_replace.offset) && //
        (!watchers_[1].has_value() || watchers_[1].value().offset != to_replace.offset))
        return;

    auto& replace_ref = (watchers_[0].value().offset == to_replace.offset) ? watchers_[0] : watchers_[1];
    auto& other_ref   = (watchers_[0].value().offset != to_replace.offset) ? watchers_[0] : watchers_[1];

    // remove the watched literal
    replace_ref = std::nullopt;

    const auto it = std::ranges::find_if(clause.get_literals(), [&vs, &other_ref](const auto& lit_id_pair) { //
        if (other_ref.has_value() && other_ref.value().offset == lit_id_pair.second.offset) {
            return false;
        }
        return get<soa_assignment>(vs[lit_id_pair.second]) == assignment::not_assigned;
    });
    if (it == clause.get_literals().end()) {
        return;
    }

    replace_ref = {vs[it->second].struct_id()};
}

std::uint8_t Clause_Watcher::size() const { return (watchers_[0].has_value() ? 1 : 0) + (watchers_[1].has_value() ? 1 : 0); }

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
            int num_variables = 0;
            int num_clauses   = 0;

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
                if (i == 0)
                    log_info("SAT solver cannot find solution for mode : UNSATISFIABLE");
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