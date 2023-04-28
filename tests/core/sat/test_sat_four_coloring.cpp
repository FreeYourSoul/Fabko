// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 27/04/23. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <catch2/catch_test_macros.hpp>

#include <sat/solver.hh>

/**
 * Representation of the 4 coloring problem: https://en.wikipedia.org/wiki/Four_color_theorem
 *
 * Map with only 2 regions
 *
 * # Assign at least one colour to region 1
* R1 B1 G1 Y1
 * # But no more than one colour
 * ~R1 ~B1
 * ~R1 ~G1
 * ~R1 ~Y1
 * ~B1 ~G1
 * ~B1 ~Y1
 * ~G1 ~Y1
 * # Similarly for region 2
 * R2 B2 G2 Y2
 * ~R2 ~B2
 * ~R2 ~G2
 * ~R2 ~Y2
 * ~B2 ~G2
 * ~B2 ~Y2
 * ~G2 ~Y2
 * # Make sure regions 1 and 2 are not coloured the same since they are neighbours
 * ~R1 ~R2
 * ~B1 ~B2
 * ~G1 ~G2
 * ~Y1 ~Y2
 */

TEST_CASE("sat_solver_4_coloring_problem") {

  fabko::sat::solver s{fabko::sat::solver_config{}};

  s.add_variables(8);

  // region 1 and 2 with color R G B Y

  // var :: 1    2    3    4
  // var :: R1   B1   G1   Y1
  // ------------------------
  // var :: 5    6    7    8
  // var :: R2   B2   G2   Y2

  // only one color can be set  region 1
  s.add_clause({fabko::sat::literal{1, false}, fabko::sat::literal{2, false}});
  s.add_clause({fabko::sat::literal{1, false}, fabko::sat::literal{3, false}});
  s.add_clause({fabko::sat::literal{1, false}, fabko::sat::literal{4, false}});
  s.add_clause({fabko::sat::literal{2, false}, fabko::sat::literal{3, false}});
  s.add_clause({fabko::sat::literal{2, false}, fabko::sat::literal{4, false}});
  s.add_clause({fabko::sat::literal{3, false}, fabko::sat::literal{4, false}});

  // only one color can be set region 2
  s.add_clause({fabko::sat::literal{5, false}, fabko::sat::literal{6, false}});
  s.add_clause({fabko::sat::literal{5, false}, fabko::sat::literal{7, false}});
  s.add_clause({fabko::sat::literal{5, false}, fabko::sat::literal{8, false}});
  s.add_clause({fabko::sat::literal{6, false}, fabko::sat::literal{7, false}});
  s.add_clause({fabko::sat::literal{7, false}, fabko::sat::literal{8, false}});
  s.add_clause({fabko::sat::literal{8, false}, fabko::sat::literal{8, false}});

  // make sure that region 1 and 2 are not coloured the same as they are neighbours
  s.add_clause({fabko::sat::literal{1, false}, fabko::sat::literal{5, false}});
  s.add_clause({fabko::sat::literal{2, false}, fabko::sat::literal{6, false}});
  s.add_clause({fabko::sat::literal{3, false}, fabko::sat::literal{7, false}});
  s.add_clause({fabko::sat::literal{4, false}, fabko::sat::literal{8, false}});

  s.solve(42); // find all solutions

  // 12 solutions are possible to this problem
  // R1 B2
  // R1 G2
  // R1 Y2
  // G1 R2
  // G1 B2
  // G1 Y2
  // B1 R2
  // B1 G2
  // B1 Y2
  // Y1 R2
  // Y1 B2
  // Y1 G2

  CHECK(s.solving_status() == fabko::sat::solver_status::SAT);
  auto r = s.results();
  CHECK(r.size() == 12);

}