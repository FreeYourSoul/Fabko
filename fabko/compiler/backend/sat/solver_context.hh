//
// Created by Quentin on 25/05/2025.
//

#ifndef SOLVER_CONTEXT_HH
#define SOLVER_CONTEXT_HH

#include <bitset>
#include <cstdint>
#include <vector>

namespace fabko::compiler::sat {
struct model;

namespace impl_details {
class solver_solution;
class literal_assigned;
class clause_watcher;
} // namespace impl_details

/**
 * @brief Represents the context for managing the state of a SAT solver
 *
 * This class is responsible for maintaining the solver's current state, its trail of
 * assigned literals, watched clauses, solutions found, and configuration settings.
 * It serves as the central context for the solver's internal operations and behavior.
 *
 * @note clause can be learned using Conflict-Driven Clause Learning (CDCL) techniques
 */
struct solver_context {
    struct configuration {

        //! after a certain number of conflicts, restart the resolution of the sat solver to avoid the algorithm to
        //! being stuck in a bad path of the resolution domain.
        std::uint32_t restart_threshold;
        std::uint32_t restart_multiplier; //!< multiplier that is applied on the threshold when hit

        // VSIDS (Variable State Independent Decaying Sum) configurations

        //! value used for the increment of the vsids value in case a conflict occurs
        std::int32_t vsids_increment = 10;
        std::int32_t decay_interval  = 100;  //!< number of ticks before decaying the vsids (to favor recent conflict)
        float vsids_decay_ratio      = 0.95; //!< ratio to decrease the importance of the vsids value over time
    };

    explicit solver_context(const model& model);

    std::vector<impl_details::solver_solution> solutions_found_;

    std::vector<impl_details::literal_assigned> trail_;
    std::vector<impl_details::clause_watcher> clauses_watcher_;
    std::size_t conflict_count_;
    std::size_t current_decision_level_;

    configuration config_{};
};

} // namespace fabko::compiler::sat

#endif //SOLVER_CONTEXT_HH
