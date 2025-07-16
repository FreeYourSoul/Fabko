// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by fys on 23.06.25. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#ifndef METADATA_H
#define METADATA_H

#include <optional>

namespace fabko::compiler {
namespace fabl {
class compiler_generation_context {}; //@todo transform into a forward declaration when class exists in compiler
}; // namespace fabl
namespace sat {
class solving_generation_context {}; //@todo transform into a forward declaration when class exists in compiler
} // namespace sat
} // namespace fabko::compiler

namespace fabko::compiler {

class metadata {
    std::optional<fabl::compiler_generation_context> fabl_generation_ctx; //!< set if the fabl compiler generated this context
    std::optional<sat::solving_generation_context> sat_generation_ctx;    //!< set if the sat generation generated this context
};

} // namespace fabko::compiler

#endif // METADATA_H
