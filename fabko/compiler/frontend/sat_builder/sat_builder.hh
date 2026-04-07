// Dual Licensing Either:
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by fys on 06.04.26. @Copyright Licensing 2022-2026
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#ifndef FABKO_SAT_BUILDER_HH
#define FABKO_SAT_BUILDER_HH

#include <vector>

#include "compiler/backend/sat/solver.hh"

namespace fabko::compiler::sat_builder {

struct sat_build_atom {
    std::vector<sat::literal> literals;
    std::vector<std::vector<sat::literal>> cnf_clauses;

    fabl::compiler_generation_context ctx;
};

class sat_model_builder {
  public:
    sat_model_builder& enrich(const sat_build_atom& atom) { return *this; }

    std::vector<sat::literal> build();

  private:
    std::size_t literal_counts_ = 0;
};

} // namespace fabko::compiler::sat_builder

#endif // FABKO_SAT_BUILDER_HH
