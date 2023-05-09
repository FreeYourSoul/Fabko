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

namespace fl        = fabko::logic;
static const auto e = fl::make_formula;

TEST_CASE("test_formula :: express_formula_string :: basic logic symbol") {
  auto dada  = fl::variable{"dada"};
  auto dede  = fl::variable{"dede"};
  auto choco = fl::variable{"choco"};

  SECTION("test disjunction") {
    auto form = e(dada, fl::disjunction, dede);

    std::string form_str = express_formula_string(form);
    CHECK("(dada ∨ dede)" == form_str);
  }

  SECTION("test disjunction disjunction") {
    auto form = e(
        e(dada, fl::disjunction, dede), fl::disjunction, choco);

    std::string form_str = express_formula_string(form);
    CHECK("((dada ∨ dede) ∨ choco)" == form_str);
  }

  SECTION("test conjunction") {
    auto form = e(dada, fl::conjunction, dede);

    std::string form_str = express_formula_string(form);
    CHECK("(dada ∧ dede)" == form_str);
  }

  SECTION("test conjunction conjunction") {
    auto form = e(
        e(dada, fl::conjunction, dede), fl::conjunction, choco);

    std::string form_str = express_formula_string(form);
    CHECK("((dada ∧ dede) ∧ choco)" == form_str);
  }

  SECTION("test conjunction disjunction") {
    auto form = e(
        e(dada, fl::conjunction, dede), fl::disjunction, choco);

    std::string form_str = express_formula_string(form);
    CHECK("((dada ∧ dede) ∨ choco)" == form_str);
  }
}