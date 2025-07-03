//
// Created by Quentin on 25/05/2025.
//

#ifndef SOLVER_CONTEXT_HH
#define SOLVER_CONTEXT_HH

#include <cstdint>
#include <vector>

#include <fil/datastructure/soa.hh>

#include "compiler/backend/metadata.hh"

//
// Forward declarations
namespace fabko::compiler::sat {

class assignment_context;

struct statistics;
class clause;
class literal;
class clause_watcher;
enum class assignment;
struct model;
} // namespace fabko::compiler::sat
// end forward declarations
//

namespace fabko::compiler::sat {

/**
 * @brief Solution returned by the sat solver
 */
class solver_solution {
    std::vector<literal> literals_solving_;                                     //!< literals that solve the SAT problem
};

using var_soa    = fil::soa<literal, assignment, assignment_context, metadata>; //!< structure of arrays representing a variable
using clause_soa = fil::soa<clause, clause_watcher, metadata>;                  //!< structure of arrays representing a clause

enum var_values {
    soa_literal          = 0,
    soa_assignment       = 1,
    soa_assignment_ctx   = 2,
    soa_var_compiler_ctx = 3,
};

enum clause_values {
    soa_clause              = 0,
    soa_watcher             = 1,
    soa_clause_compiler_ctx = 2,
};

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
        // @todo check in tuto what are the recommended values for start
        std::uint32_t restart_threshold {100};
        std::uint32_t restart_multiplier {2}; //!< multiplier that is applied on the threshold when hit

        // VSIDS (Variable State Independent Decaying Sum) configurations

        //! value used for the increment of the vsids value in case a conflict occurs
        std::int32_t vsids_increment {10};
        std::int32_t decay_interval {100}; //!< number of ticks before decaying the vsids (to favor recent conflict)
        float vsids_decay_ratio {0.95};    //!< ratio to decrease the importance of the vsids value over time
    };

    struct statistics {
        std::size_t restarts;     //!< number of restarts that occurred
        std::size_t conflicts;    //!< number of conflicts that occurred in the overall execution of the solver
        std::size_t propagations; //!< amount of propagation that occurred
        std::size_t decisions;    //!< number of decisions taken
        std::size_t backtracks;   //!< number of backtracking that occurred
    };

    explicit solver_context(const model& model);

    configuration config_ {};                           //!< configuration of the solver
    std::reference_wrapper<const model> model_;         //!< reference to the model being solved

    var_soa vars_soa_;                                  //!< variables of the SAT solver, containing their assignment and context
    clause_soa clauses_soa_;                            //!< clauses of the SAT solver, containing their watchers and context
    std::vector<var_soa::struct_id> trail_ {};          //!< trail of assigned literals and their context

    std::size_t conflict_count_since_last_restart_ {0}; //!< current number of conflicts since last restart
    std::size_t current_decision_level_ {0};

    statistics statistics_ {};                          //!< resolution statistics of the solver

    std::vector<solver_solution> solutions_found_ {};   //!< final solutions found by the solver
};

} // namespace fabko::compiler::sat

#endif // SOLVER_CONTEXT_HH
