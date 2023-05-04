// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 04/05/23. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <catch2/catch_test_macros.hpp>

#include <logic/formula.hh>

static const auto e   = fabko::logic::make_formula;
namespace fl          = fabko::logic;

TEST_CASE("test_formula :: basic logic symbol") {
  auto dada  = fabko::logic::variable{"dada"};
  auto dede  = fabko::logic::variable{"dede"};
  auto choco = fabko::logic::variable{"choco"};

  SECTION("test disjunction") {
    auto form = e(dada, fl::disjunction, dede);

    std::string form_str = form->express_cnf();
  }

  SECTION("test disjunction disjunction") {
    auto form = e(
        e(dada, fl::disjunction, dede), fl::disjunction, choco);

    std::string form_str = form->express_cnf();
  }
}