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

#ifndef SOLVER_HH
#define SOLVER_HH

#include <memory>
#include <vector>

#include "common/exception.hh"
#include "solver_context.hh"

#include <filesystem>
#include <ranges>

namespace fabko::compiler::sat {

enum class assignment {
    on,          //!< Literal is assigned to true
    off,         //!< Literal is assigned to false
    not_assigned //!< Literal is not assigned yet
};
[[nodiscard]] std::string to_string(assignment a);

class Literal {
  public:
    explicit Literal(std::int64_t value)
        : value_(value) {}

    bool operator==(const Literal& lit) const { return std::abs(value_) == std::abs(lit.value_); }
    auto operator<=>(const Literal& lhs) const { return std::abs(value_) <=> std::abs(lhs.value_); }

    [[nodiscard]] bool is_on() const { return value_ > 0; }
    [[nodiscard]] bool is_off() const { return value_ < 0; }

    /**
     * @return literal value (absolute value that represent the variable)
     */
    [[nodiscard]] std::int64_t value() const { return std::abs(value_); }

    [[nodiscard]] friend std::string to_string(const Literal& lit) { return std::to_string(lit.value_); }

  private:
    std::int64_t value_;
};

class Clause {
    friend class clause_view;

  public:
    Clause(std::vector<Literal> clause, std::vector<Vars_Soa::struct_id> literals_mapping)
        : vars_([&]() {
            return std::views::zip_transform(
                       [](auto lit_clause, auto lit_mapping) { //
                           return std::make_pair(lit_clause, lit_mapping);
                       },
                       clause,
                       literals_mapping)
                 | std::ranges::to<std::vector<std::pair<Literal, Vars_Soa::struct_id>>>();
        }()) {}

    explicit Clause(std::vector<std::pair<Literal, Vars_Soa::struct_id>>&& lit_vars_mapping)
        : vars_(std::move(lit_vars_mapping)) {}

    [[nodiscard]] bool is_empty() const { return vars_.empty(); }
    [[nodiscard]] const std::vector<std::pair<Literal, Vars_Soa::struct_id>>& get_literals() const { return vars_; }
    [[nodiscard]] friend std::string to_string(const Clause& clause) {
        return std::ranges::fold_left(clause.vars_, std::string {"clause["}, [](std::string res, const auto& lit_it) { //
            return std::format("{}{},", res, to_string(lit_it.first));
        }) + "]";
    }

  private:
    std::int64_t id_ {0};
    std::vector<std::pair<Literal, Vars_Soa::struct_id>> vars_; //!< pair of a clause literal and the variable id it refers to in the soa_struct

    std::shared_ptr<metadata> debug_info_;
};

/**
 * @brief Class implementing the 2-watched literals scheme for efficient clause monitoring
 *
 * This class implements the 2-watched literals scheme, a key optimization in modern SAT solvers.
 * It maintains two "watcher" literals per clause (except for unit clauses which have one),
 * allowing for efficient propagation and backtracking during the solving process.
 *
 * The watchers are initially set to the first and last literals in the clause. Only when both
 * watched literals become false does the solver need to examine the entire clause.
 */
class clause_watcher {
  public:
    /**
     * @brief Constructs a clause watcher for the given clause
     * @param vs The variable structure-of-arrays containing all variables
     * @param clause The clause to watch
     * @throws fabko_exception if the clause is empty
     */
    explicit clause_watcher(const Vars_Soa& vs, const Clause& clause)
        : watchers_([&]() -> std::vector<Literal> { //
            using std::get;
            fabko_assert(!clause.get_literals().empty(), "Cannot make a clause watchers over an empty clause");
            if (clause.get_literals().size() == 1) {
                return {get<soa_literal>(vs[clause.get_literals().front().second])};
            }
            return {get<soa_literal>(vs[clause.get_literals().front().second]), get<soa_literal>(vs[clause.get_literals().back().second])};
        }()) {}

  private:
    std::vector<Literal> watchers_; //!< The watched literals (1 for unit clauses, 2 for other clauses)
};

/**
 * @brief Represents the context in which an assignment occurs on a literal
 *
 * This class maintains the state of an assigned literal in the SAT solver, including its decision level, VSIDS activity score, and the reason for its assignment
 * (Whether it was a decision or propagated through a clause.)
 *
 */
class Assignment_Context {
  public:
    enum class type_assign {
        decision,
        propagated,
    };

    /**
     * @return true if the assignment is propagation from a clause, false if it is a decision from the SAT solver
     */
    [[nodiscard]] bool is_propagated() const { return clause_propagation_.has_value(); }
    /**
     * @return true if the assignment is a decision made by the SAT solver, false if it is propagation from a clause.
     */
    [[nodiscard]] bool is_decision() const { return !is_propagated(); }

    std::int64_t vsids_activity_ {};                              //!< VSIDS (Variable State Independent Decaying Sum) activity value type
    std::size_t decision_level_ {};                               //!< decision level of the literal
    std::optional<Clauses_Soa::struct_id> clause_propagation_ {}; //!< clause that produced this (std::nullopt if decision type)
};

struct conflict_resolution_result {
    Clause learned_clause;          //!< clause learned from conflict resolution
    std::size_t backtrack_level {}; //!< level the conflict resolution found to requires the solver to backtrack to
};

struct model {
    std::vector<Literal> literals;
    std::vector<std::vector<Literal>> clauses;

    //@todo add compiler information linked for each clauses and literals that generated that model

    Solver_Context::configuration conf;
};

/**
 * @brief Create a model from a DNF file.
 *
 * This function reads a DNF (Disjunctive Normal Form) file and constructs a model that can be used by the SAT solver. The DNF file should contain clauses in the
 * appropriate format.
 *
 * @param dnf_file Path to the DNF file to be processed.
 * @return A model object representing the parsed DNF file.
 */
model make_model_from_cnf_file(const std::filesystem::path& dnf_file);

enum class sat_error {
    unsatisfiable, //!< The SAT problem is unsatisfiable.
    error          //!< An error occurred during the solving process.
};

/**
 * @brief The solver class for solving SAT models.
 *
 * This class provides functionality to solve a satisfiability problem given a model. It uses
 * an internal solver_context to handle the solving process and produces results as a list of
 * literal assignments.
 */
class solver {
  public:
    struct result {
        std::vector<Literal> literals;
    };

    explicit solver(model m);

    std::vector<result> solve(std::int32_t expected = -1);

  private:
    Solver_Context context_; // !< The context for the solver, containing configuration and state.
    model model_;
};

} // namespace fabko::compiler::sat

#endif // SOLVER_HH
