// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <catch2/catch_test_macros.hpp>

#include "logic/sat/assignment_bitset.hh"

TEST_CASE("bitset") {
  std::bitset<5> d{};

  REQUIRE_FALSE(d[0]);
  REQUIRE_FALSE(d[1]);
  REQUIRE_FALSE(d[2]);
  REQUIRE_FALSE(d[3]);
  REQUIRE_FALSE(d[4]);
}

TEST_CASE("assignment_bitset") {

  using namespace fabko::sat;

  fabko::assignment_bitset<5> bitset{};

  REQUIRE(bitset.chunk_size() == 5);

  SECTION("failure cur_assignment of var before reserve variable") {
    REQUIRE_THROWS(bitset.assign_variable(variable{1}, true));
  }

  SECTION("failure cur_assignment of var before reserve") {
    REQUIRE_THROWS(bitset.unassign_variable(variable{1}));
  }

  SECTION("failure check of var before reserve") {
    REQUIRE_THROWS(bitset.check_assignment(variable{1}));
  }

  SECTION("failure reserving 0 var") {
    REQUIRE_THROWS(bitset.reserve_new_variable(0z));
  }

  SECTION("reserve of 4 var") {
    REQUIRE(bitset.nb_vars() == 0z);
    bitset.reserve_new_variable(2);
    REQUIRE(bitset.nb_vars() == 2z);
    bitset.reserve_new_variable(2);
    REQUIRE(bitset.nb_vars() == 4z);

    REQUIRE_FALSE(bitset.check_assignment(1).has_value());
    REQUIRE_FALSE(bitset.check_assignment(2).has_value());
    REQUIRE_FALSE(bitset.check_assignment(3).has_value());
    REQUIRE_FALSE(bitset.check_assignment(4).has_value());

    SECTION("cur_assignment works") {
      bitset.assign_variable(variable{1}, true);
      bitset.assign_variable(variable{2}, false);
      bitset.assign_variable(variable{3}, true);
      bitset.assign_variable(variable{4}, false);

      REQUIRE(bitset.check_assignment(1).has_value());
      REQUIRE(bitset.check_assignment(2).has_value());
      REQUIRE(bitset.check_assignment(3).has_value());
      REQUIRE(bitset.check_assignment(4).has_value());

      REQUIRE(bitset.check_assignment(1).value());
      REQUIRE_FALSE(bitset.check_assignment(2).value());
      REQUIRE(bitset.check_assignment(3).value());
      REQUIRE_FALSE(bitset.check_assignment(4).value());
    }

    SECTION("5th var cannot be allocated") {
      REQUIRE_THROWS(bitset.check_assignment(5).has_value());
      REQUIRE_THROWS(bitset.assign_variable(variable{5}, true));
      REQUIRE_THROWS(bitset.unassign_variable(variable{5}));
    }

    SECTION("5th var allocation doesn't trigger a new chunk") {

      // only one chunk is in use currently
      REQUIRE(bitset.nb_vars() == 4);
      REQUIRE(bitset.nb_chunks() == 1);

      // add 1 variables
      bitset.reserve_new_variable(1);

      REQUIRE(bitset.nb_vars() == 5);
      REQUIRE_FALSE(bitset.check_assignment(5).has_value());

      // still only one chunk as the 5th element is added a chunk-size of 5
      REQUIRE(bitset.nb_chunks() == 1);
    }

    SECTION("6th var allocation trigger a new chunk") {
      // only one chunk is in use currently
      REQUIRE(bitset.nb_vars() == 4);
      REQUIRE(bitset.nb_chunks() == 1);

      // add 2 variables
      bitset.reserve_new_variable(2);

      REQUIRE(bitset.nb_vars() == 6);
      REQUIRE_FALSE(bitset.check_assignment(5).has_value());

      // 2 chunks as the 6th element is added a chunk-size of 5
      REQUIRE(bitset.nb_chunks() == 2);
    }

    SECTION("validate all assigned") {
      REQUIRE_FALSE(bitset.check_assignment(1).has_value());
      REQUIRE_FALSE(bitset.check_assignment(2).has_value());
      REQUIRE_FALSE(bitset.check_assignment(3).has_value());
      REQUIRE_FALSE(bitset.check_assignment(4).has_value());

      REQUIRE_FALSE(bitset.all_assigned());
      REQUIRE(bitset.number_assigned() == 0);

      bitset.assign_variable(1, true);
      bitset.assign_variable(3, false);

      REQUIRE(bitset.check_assignment(1).has_value());
      REQUIRE_FALSE(bitset.check_assignment(2).has_value());
      REQUIRE(bitset.check_assignment(3).has_value());
      REQUIRE_FALSE(bitset.check_assignment(4).has_value());

      REQUIRE_FALSE(bitset.all_assigned());
      REQUIRE(bitset.number_assigned() == 2);

      bitset.assign_variable(2, true);
      bitset.assign_variable(4, false);

      REQUIRE(bitset.check_assignment(1).has_value());
      REQUIRE(bitset.check_assignment(2).has_value());
      REQUIRE(bitset.check_assignment(3).has_value());
      REQUIRE(bitset.check_assignment(4).has_value());

      REQUIRE(bitset.all_assigned());
      REQUIRE(bitset.number_assigned() == 4);

      bitset.unassign_variable(4);

      REQUIRE(bitset.check_assignment(1).has_value());
      REQUIRE(bitset.check_assignment(2).has_value());
      REQUIRE(bitset.check_assignment(3).has_value());
      REQUIRE_FALSE(bitset.check_assignment(4).has_value());

      REQUIRE_FALSE(bitset.all_assigned());
      REQUIRE(bitset.number_assigned() == 3);
    }
  }

} // End TestCase : assignment_bitset
