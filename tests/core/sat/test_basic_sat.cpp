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

#include <catch2/catch_test_macros.hpp>

#include <ranges>
#include <set>

#include <sat/solver.hh>

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


TEST_CASE("basic_case_sat_solver_0_solution") {

  fabko::sat::solver s{fabko::sat::solver_config{}};

  // Variable 1 defined
  s.add_variables(1);


  SECTION("Default :: Ask all solution :: find no solution") {
    // DIMACS
    //   1  2
    //   1  0
    //  ~1  0

    // expected result : UNSAT

    // clause
    //  1  0
    s.add_clause({fabko::sat::literal{1, true}});

    // clause
    // ~1  0
    s.add_clause({fabko::sat::literal{1, false}});

    CHECK(s.solving_status() == fabko::sat::solver_status::BUILDING);
    s.solve();
    CHECK(s.solving_status() == fabko::sat::solver_status::UNSAT);
    auto result = s.results();
    CHECK(result.empty());
  }

}


TEST_CASE("basic_case_sat_solver_2_clauses_2_solution") {

  fabko::sat::solver s{fabko::sat::solver_config{}};

  // Variable 1 2 3 defined
  s.add_variables(3);

  // DIMACS
  //   3  2
  //   1     0
  //  ~1 ~2  0

  // expected result : 1 ~2 3 |  1 ~2 ~3

  // clause
  // 3 0
  s.add_clause({fabko::sat::literal{1, true}});

  // clause
  //  ~1 ~2 0
  s.add_clause(
      {fabko::sat::literal{1, false},
       fabko::sat::literal{2, false}});


  SECTION("Default :: Ask all solution :: find two solution") {
    CHECK(s.solving_status() == fabko::sat::solver_status::BUILDING);

    s.solve();

    CHECK(s.solving_status() == fabko::sat::solver_status::SAT);

    auto result = s.results();
    CHECK(result.size() == 2);
    CHECK(bool(result[0].get_all()[0]));
    CHECK_FALSE(bool(result[0].get_all()[1]));
    CHECK_FALSE(bool(result[0].get_all()[2]));

    CHECK(bool(result[1].get_all()[0]));
    CHECK(bool(result[1].get_all()[1]));
    CHECK_FALSE(bool(result[1].get_all()[2]));
  }

  SECTION("Custom :: Ask one solution :: find one solution") {
    CHECK(s.solving_status() == fabko::sat::solver_status::BUILDING);

    s.solve(1);

    CHECK(s.solving_status() == fabko::sat::solver_status::SAT);

    auto result = s.results();
    CHECK(result.size() == 1);
    CHECK(bool(result[0].get_all()[0]));
    CHECK_FALSE(bool(result[0].get_all()[1]));
    CHECK_FALSE(bool(result[0].get_all()[2]));
  }

  SECTION("Custom :: Ask 42 solution :: find two solutions") {
    CHECK(s.solving_status() == fabko::sat::solver_status::BUILDING);

    s.solve(42);

    CHECK(s.solving_status() == fabko::sat::solver_status::SAT);

    auto result = s.results();
    CHECK(result.size() == 2);
    CHECK(bool(result[0].get_all()[0]));
    CHECK_FALSE(bool(result[0].get_all()[1]));
    CHECK_FALSE(bool(result[0].get_all()[2]));

    CHECK(bool(result[1].get_all()[0]));
    CHECK(bool(result[1].get_all()[1]));
    CHECK_FALSE(bool(result[1].get_all()[2]));
  }
}


TEST_CASE("basic_case_sat_solver_3_clause_1_solutions") {

  fabko::sat::solver s{fabko::sat::solver_config{}};

  // Variable 1 2 3 defined
  s.add_variables(3);

  // DIMACS
  //   3  3
  //   1  2  0
  //   3 ~2  0
  //  ~3     0

  // expected result : 1 ~2 ~3

  // clause
  //  1  2  0
  s.add_clause(
      {fabko::sat::literal{1, true},
       fabko::sat::literal{2, true}});

  //  3  ~2  0
  s.add_clause(
      {fabko::sat::literal{3, true},
       fabko::sat::literal{2, false}});

  //  ~3  0
  s.add_clause(
      {fabko::sat::literal{3, false}});


  SECTION("Custom :: Ask one solution :: find one solutions") {
    CHECK(s.solving_status() == fabko::sat::solver_status::BUILDING);

    s.solve(1);

    CHECK(s.solving_status() == fabko::sat::solver_status::SAT);

    auto result = s.results();
    CHECK(result.size() == 1);
    CHECK(bool(result[0].get_all()[0]));
    CHECK_FALSE(bool(result[0].get_all()[1]));
    CHECK_FALSE(bool(result[0].get_all()[2]));
  }


  SECTION("Custom :: Ask 42 solution :: find one solutions") {
    CHECK(s.solving_status() == fabko::sat::solver_status::BUILDING);

    s.solve(42);

    CHECK(s.solving_status() == fabko::sat::solver_status::SAT);

    auto result = s.results();
    CHECK(result.size() == 1);
    CHECK(bool(result[0].get_all()[0]));
    CHECK_FALSE(bool(result[0].get_all()[1]));
    CHECK_FALSE(bool(result[0].get_all()[2]));
  }

  SECTION("Default :: Ask all solution :: find one solutions") {
    CHECK(s.solving_status() == fabko::sat::solver_status::BUILDING);

    s.solve();

    CHECK(s.solving_status() == fabko::sat::solver_status::SAT);

    auto result = s.results();
    CHECK(result.size() == 1);
    CHECK(bool(result[0].get_all()[0]));
    CHECK_FALSE(bool(result[0].get_all()[1]));
    CHECK_FALSE(bool(result[0].get_all()[2]));
  }
}

