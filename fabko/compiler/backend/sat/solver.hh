//
// Created by Quentin on 25/05/2025.
//

#ifndef SOLVER_HH
#define SOLVER_HH

#include <bitset>
#include <memory>
#include <vector>

#include "solver_context.hh"

namespace fabko::compiler {
class compiler_context;
}

namespace fabko::compiler::sat {

class literal {
    friend class literal;

  public:
    explicit literal(std::int64_t value, std::shared_ptr<compiler_context> debug_info = nullptr)
        : value_(std::abs(value)), debug_info_(std::move(debug_info)) {}

  private:
    std::int64_t value_;
    std::shared_ptr<compiler_context> debug_info_;
};

class clause {
    friend class clause_view;

  public:
    explicit clause(std::vector<literal> literals) : literals_(std::move(literals)) {}

    [[nodiscard]] bool is_empty() const { return literals_.empty(); }
    [[nodiscard]] const std::vector<literal>& literals() const { return literals_; }

  private:
    std::int64_t id_{0};
    std::vector<literal> literals_;

    std::shared_ptr<compiler_context> debug_info_;
};

class clause_view {
  public:
    explicit clause_view(const clause& literal) : clause_(literal) {}

  private:
    std::reference_wrapper<const clause> clause_;
};

struct model {
    std::vector<clause> clauses;
    std::vector<literal> literals;

    solver_context::configuration conf;
};

enum class sat_error {
    unsatisfiable,   //!< The SAT problem is unsatisfiable.
    error           //!< An error occurred during the solving process.
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

#endif //SOLVER_HH
