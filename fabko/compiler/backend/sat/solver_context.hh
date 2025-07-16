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

#ifndef SOLVER_CONTEXT_HH
#define SOLVER_CONTEXT_HH

#include <cstdint>
#include <vector>

#include <fil/datastructure/soa.hh>

#include "compiler/backend/metadata.hh"

//
// Forward declarations
namespace fabko::compiler::sat {

class Assignment_Context;

struct statistics;
class Clause;
class Literal;
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
class Solver_Solution {
    std::vector<Literal> literals_solving_;                                      //!< literals that solve the SAT problem
};

using Vars_Soa    = fil::soa<Literal, assignment, Assignment_Context, metadata>; //!< structure of arrays representing a variable
using Clauses_Soa = fil::soa<Clause, clause_watcher, metadata>;                  //!< structure of arrays representing a clause

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
struct Solver_Context {
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
        std::size_t restarts;         //!< number of restarts that occurred
        std::size_t conflicts;        //!< number of conflicts that occurred in the overall execution of the solver
        std::size_t propagations;     //!< amount of propagation that occurred
        std::size_t decisions;        //!< number of decisions taken
        std::size_t backtracks;       //!< number of backtracking that occurred
        std::size_t learned_clause;   //!< number of clauses learned through the CDCL
        std::size_t max_decision_lvl; //!< level of decision maximum during sat solver
    };

    explicit Solver_Context(const model& model);

    configuration config_ {};                   //!< configuration of the solver
    std::reference_wrapper<const model> model_; //!< reference to the model being solved

    Vars_Soa vars_soa_;                         //!< variables of the SAT solver, containing their assignment and context
    Clauses_Soa clauses_soa_;                   //!< clauses of the SAT solver, containing their watchers and context

    //! trail of assigned literals and their context
    //! the trail store in an ordered fashion all the variables that has been assigned during sat resolution. the level of assignment of the literal
    //! is the indicator of propagation against decision
    std::vector<Vars_Soa::struct_id> trail_ {};

    std::size_t conflict_count_since_last_restart_ {0}; //!< current number of conflicts since last restart
    std::size_t current_decision_level_ {0};

    statistics statistics_ {};                          //!< resolution statistics of the solver

    std::vector<Solver_Solution> solutions_found_ {};   //!< final solutions found by the solver
};

} // namespace fabko::compiler::sat

#endif // SOLVER_CONTEXT_HH
