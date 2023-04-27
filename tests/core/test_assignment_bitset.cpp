// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <catch2/catch.hpp>

#include <sat/assignment_bitset.hh>
#include <sat/solver.hh>

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

  SECTION("failure cur_assignment of variable before reserve") {
    REQUIRE_THROWS(bitset.assign_variable(variable{1}, true));
  }

  SECTION("failure cur_assignment of variable before reserve") {
    REQUIRE_THROWS(bitset.unassign_variable(variable{1}));
  }

  SECTION("failure check of variable before reserve") {
    REQUIRE_THROWS(bitset.check_assignment(variable{1}));
  }

  SECTION("failure reserving 0 variable") {
    REQUIRE_THROWS(bitset.reserve_new_variable(0z));
  }

  SECTION("reserve of 4 variable") {
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

    SECTION("5th variable cannot be allocated") {
      REQUIRE_THROWS(bitset.check_assignment(5).has_value());
      REQUIRE_THROWS(bitset.assign_variable(variable{5}, true));
      REQUIRE_THROWS(bitset.unassign_variable(variable{5}));
    }

    SECTION("5th variable allocation doesn't trigger a new chunk") {

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

    SECTION("6th variable allocation trigger a new chunk") {
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
  }

}// End TestCase : assignment_bitset
