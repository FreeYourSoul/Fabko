//
// Created by Quentin on 25/05/2025.
//

#ifndef SOLVER_HH
#define SOLVER_HH

#include <memory>
#include <vector>

#include "common/exception.hh"
#include "solver_context.hh"

#include <filesystem>

namespace fabko::compiler::sat {

enum class assignment {
    on,          //!< Literal is assigned to true
    off,         //!< Literal is assigned to false
    not_assigned //!< Literal is not assigned yet
};

class literal {
  public:
    explicit literal(std::int64_t value)
        : value_(std::abs(value)) {}

    auto operator<=>(const literal& lhs) const = default;

    /**
     * @return literal value (absolute value that represent the variable)
     */
    [[nodiscard]] std::int64_t value() const { return std::abs(value_); }

  private:
    std::int64_t value_;
};

class clause {
    friend class clause_view;

  public:
    explicit clause(std::vector<var_soa::struct_id> literals)
        : vars_(std::move(literals)) {}

    [[nodiscard]] bool is_empty() const { return vars_.empty(); }
    [[nodiscard]] const std::vector<var_soa::struct_id>& vars() const { return vars_; }

  private:
    std::int64_t id_ {0};
    std::vector<var_soa::struct_id> vars_;

    std::shared_ptr<compiler_context> debug_info_;
};

class clause_view {
  public:
    explicit clause_view(const clause& literal)
        : clause_(literal) {}

  private:
    std::reference_wrapper<const clause> clause_;
};

class clause_watcher {
  public:
    explicit clause_watcher(const var_soa& vs, const clause& clause)
        : watchers_([&]() -> std::vector<literal> { //
            using std::get;
            fabko_assert(clause.vars().empty(), "Cannot make a clause watchers over an empty clause");
            if (clause.vars().size() == 1) {
                return {get<soa_literal>(vs[clause.vars().front()])};
            }
            return {get<soa_literal>(vs[clause.vars().front()]), get<soa_literal>(vs[clause.vars().back()])};
        }()) {}

  private:
    std::vector<literal> watchers_;
};

/**
 * @brief Represents the context in which an assignement occurs on a literal
 *
 * This class maintains the state of an assigned literal in the SAT solver, including
 * its decision level, VSIDS activity score, and the reason for its assignment
 * (Whether it was a decision or propagated through a clause.)
 *
 */
class assignment_context {
  public:
    enum class type_assign {
        decision,
        propagated,
    };

    [[nodiscard]] bool is_decision() const { return !clause_propagation_.has_value(); }
    [[nodiscard]] bool is_propagated() const { return clause_propagation_.has_value(); }

    std::int64_t vsids_activity_ {};                   //!< VSIDS (Variable State Independent Decaying Sum) activity value type
    std::size_t decision_level_ {};                    //!< decision level of the literal
    std::optional<clause_view> clause_propagation_ {}; //!< clause that produced this (nullopt if decision type)
};

struct conflict_resolution_result {
    clause learned_clause;          //!< clause learned from conflict resolution
    std::size_t backtrack_level {}; //!< level the conflict resolution found to requires the solver to backtrack to
};

struct model {
    std::vector<literal> literals;
    std::vector<std::vector<literal>> clauses;

    //@todo add compiler information linked for each clauses and literals that generated that model

    solver_context::configuration conf;
};

/**
 * @brief Create a model from a DNF file.
 *
 * This function reads a DNF (Disjunctive Normal Form) file and constructs a model
 * that can be used by the SAT solver. The DNF file should contain clauses in the
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
        std::vector<literal> literals;
    };

    explicit solver(model m);

    std::vector<result> solve(std::int32_t expected = -1);

  private:
    solver_context context_; // !< The context for the solver, containing configuration and state.
    model model_;
};

} // namespace fabko::compiler::sat

#endif // SOLVER_HH
