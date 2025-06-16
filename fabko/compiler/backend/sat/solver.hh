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
    friend class literal_view;

  public:
    explicit literal(std::int64_t value, std::shared_ptr<compiler_context> debug_info = nullptr)
        : value_(value), debug_info_(std::move(debug_info)) {}

  private:
    std::int64_t value_;
    std::shared_ptr<compiler_context> debug_info_;
};

class literal_view {

  public:
    explicit literal_view(const literal& literal) : literal_(literal) {}

    [[nodiscard]] std::int64_t get_value() const { return literal_.get().value_; }
    [[nodiscard]] std::int64_t get_literal_id() const { return std::abs(literal_.get().value_); }

  private:
    std::reference_wrapper<const literal> literal_;
};

class clause {
    friend class clause_view;

  public:
    explicit clause(std::vector<literal_view> literals) : literals_(std::move(literals)) {}

    [[nodiscard]] const std::vector<literal_view>& literals() const { return literals_; }

  private:
    std::int64_t id_{0};
    std::vector<literal_view> literals_;

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
