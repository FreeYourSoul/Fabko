// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <catch2/catch.hpp>

#include <ranges>
#include <sat/solver.hh>
#include <set>

TEST_CASE("test_literal_properties") {
  //
  // A literal is equal to variable * 2 (+ 1 if positive) or (+0 if negative)
  //

  // check for variable `1` that the property is working before looping
  fabko::sat::literal l_pos{1, true};
  CHECK(l_pos);
  CHECK(l_pos.var() == 1);
  CHECK(l_pos.value() == 1 * 2 + 1);

  fabko::sat::literal l_neg{1, false};
  CHECK_FALSE(l_neg);
  CHECK(l_neg.var() == 1);
  CHECK(l_neg.value() == 1 * 2 + 0);

  fabko::sat::literal opposite = ~l_pos;
  CHECK(opposite != l_pos);
  CHECK(opposite == l_neg);

  // check at bigger scale
  std::set<unsigned> set_check_unique{};

  for (unsigned v : std::ranges::views::iota(1u, 1001u)) {
    fabko::sat::literal l_positive_loop{v, true};
    fabko::sat::literal l_negative_loop{v, false};

    // validate boolean operator
    CHECK(l_positive_loop);
    CHECK_FALSE(l_negative_loop);

    // validate that the property of the variable is working appropriately (even on multiple execution)
    CHECK(l_positive_loop.value() == v * 2 + 1);
    CHECK(l_positive_loop.value() == v * 2 + 1);
    CHECK(l_negative_loop.value() == v * 2 + 0);
    CHECK(l_negative_loop.value() == v * 2 + 0);

    // validate that variable index can still be retrieved from the literal
    CHECK(l_positive_loop.var() == v);
    CHECK(l_negative_loop.var() == v);

    // validate that a value is never inserted twice
    const auto [it_p, success_p] = set_check_unique.insert(l_positive_loop.value());
    const auto [it_n, success_n] = set_check_unique.insert(l_negative_loop.value());
    CHECK(success_p);
    CHECK(success_n);

    fabko::sat::literal opposite_p_loop = ~l_positive_loop;
    CHECK(opposite_p_loop != l_positive_loop);
    CHECK(opposite_p_loop == l_negative_loop);

    fabko::sat::literal opposite_n_loop = ~l_negative_loop;
    CHECK(opposite_n_loop != l_negative_loop);
    CHECK(opposite_n_loop == l_positive_loop);
  }

  // validate that we did insert all the desired values in the set
  CHECK(set_check_unique.size() == 1000uz * 2);
}

TEST_CASE("basic_case_sat_solver") {

  fabko::sat::solver s{fabko::sat::solver_config{}};

  // Variable 1 2 3 defined
  s.add_variables(3);

  // DIMACS
  // 3 2
  //   1  2  3  0
  //  ~1 ~2     0

  // clause
  //  1 2 3 0
  s.add_clause(
      {fabko::sat::literal{1, true},
       fabko::sat::literal{2, true},
       fabko::sat::literal{3, true}});

  // clause
  //  ~1 ~2 0
  s.add_clause(
      {fabko::sat::literal{1, false},
       fabko::sat::literal{2, false}});

  CHECK(s.solving_status() == fabko::sat::solver_status::BUILDING);

  s.solve();

  CHECK(s.solving_status() == fabko::sat::solver_status::SAT);
}
