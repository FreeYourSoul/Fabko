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

#include "integer_sat_builder.hh"

namespace fabko::compiler::sat_builder {

namespace {

sat::literal negate(sat::literal lit) { return sat::literal {-lit.value()}; }

std::pair<std::vector<sat::literal>, std::vector<std::vector<sat::literal>>> make_integer_size_sat(std::int64_t value) {
    static constexpr sat::literal bits16 {0};
    static constexpr sat::literal bits32 {1};
    static constexpr sat::literal bits64 {2};
    static constexpr sat::literal bits_negative {3};

    std::vector<std::vector<sat::literal>> clauses {};
    clauses.push_back(value > std::numeric_limits<std::uint8_t>::max() ? std::vector {bits16} : std::vector {negate(bits16)});
    clauses.push_back(value > std::numeric_limits<std::uint16_t>::max() ? std::vector {bits32} : std::vector {negate(bits32)});
    clauses.push_back(value > std::numeric_limits<std::uint32_t>::max() ? std::vector {bits64} : std::vector {negate(bits64)});
    clauses.push_back(value >= 0 ? std::vector {negate(bits32)} : std::vector {bits32});

    return {
        std::vector {bits16, bits32, bits64, bits_negative},
         std::move(clauses)
    };
}

} // namespace

integer_sat_builder::integer_sat_builder(fabl::compiler_generation_context ctx, std::int64_t value)
    : value_(value)
    , bits_([value] -> std::uint8_t {
        if (value > std::numeric_limits<std::uint8_t>::max()) {
            return 16;
        }
        if (value > std::numeric_limits<std::uint16_t>::max()) {
            return 32;
        }
        if (value > std::numeric_limits<std::uint32_t>::max()) {
            return 64;
        }
        return 8;
    }())
    , ctx_(std::move(ctx)) {}

sat_build_atom integer_sat_builder::generate_model() {
    auto [literals, cnf_clauses] = make_integer_size_sat(value_);

    return sat_build_atom {
        .literals    = std::move(literals),
        .cnf_clauses = std::move(cnf_clauses),
    };
}

} // namespace fabko::compiler::sat_builder