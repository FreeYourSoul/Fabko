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

#ifndef FABKO_INTEGER_SAT_GENERATOR_HH
#define FABKO_INTEGER_SAT_GENERATOR_HH

#include "compiler/backend/sat/solver.hh"
#include "compiler/metadata.hh"

#include "compiler/frontend/sat_builder/sat_builder.hh"

namespace fabko::compiler::sat_builder {

class integer_sat_builder {
  public:
    integer_sat_builder(fabl::compiler_generation_context ctx, std::int64_t value);

    /**
     * @return produce an atomic element
     */
    sat_build_atom generate_model();

    /**
     * @return retrieve the context for which that integer has been built for
     */
    const fabl::compiler_generation_context& generation_context() const { return ctx_; }

  private:
    std::int64_t value_;                    //!< value to be built by the sat builder
    std::uint8_t bits_;                     //!< number of bits holding the value
    fabl::compiler_generation_context ctx_; //!< compiler context for the building of that integer SAT representation
};

} // namespace fabko::compiler::sat_builder

#endif // FABKO_INTEGER_SAT_GENERATOR_HH
