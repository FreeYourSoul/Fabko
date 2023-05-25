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
#include <fmt/core.h>

#include "logic/formula.hh"

TEST_CASE("test_formula :: basic test on set") {
    using namespace std::literals;

    SECTION("Simple setup set 3 splices") {
        auto line = fabko::logic::set(3);
        SECTION("without restriction") {
            // ABC
            // ACB
            // BAC
            // BCA
            // CAB
            // CBA
            // 3^1*1 = 6 options
        }

        SECTION("with restriction") {
            conj(line[0, 1], std::vector{"B"s});
            // BAC
            // BCA
            // only (3 - 1)^1*1 = 2 options -- -1 because one variable is fixed
        }
    }
//
//    SECTION("Splice fails") {
//        auto line = fabko::logic::set(3);
//
//        // end of splice to large
//        CHECK_THROWS(line[0, 4]);
//        // beginning of splice impossible
//        CHECK_THROWS(line[4, 5]);
//        // first value bigger than second
//        CHECK_THROWS(line[2, 0]);
//    }
//
//    SECTION("9 value set 3 splices with constraint") {
//
//        auto line = fabko::logic::set(9);
//
//        auto view_set_1 = line[0, 3];
//        conj(view_set_1, std::vector{"2"s, "4"s, "6"s, "8"s});
//
//        auto view_set_2 = line[3, 6];
//        conj(view_set_2, {"2"s});
//        disj(view_set_1, {"2"s, "1"s, "3"s, "5"s});
//
//        auto view_set_3 = line[6, 9];
//
//        // all those options are per-mutable. And thus x^2*y options is possible
//        // with x the number of variable and y the number of possible assignment
//
//        // `first` -> has to be 4 6 8 as `second` must contain 2
//        //  --> 3 variables 1 possible assignment = 3^2*1=6
//
//        // assignment second can be: 2 1 3 5 with 2 mandatory
//        // 2 1 3
//        // 2 1 5
//        // 2 3 5
//        //  --> 3 variables 3 possible assignment = 3^2*3=18 possibility
//
//        // assignment `third` can be : 1 3 5  (but not more that one of 1 3 5 otherwise
//        // `second` is impossible) and thus MUST contain 7 9
//        // 1 7 9
//        // 3 7 9
//        // 5 7 9
//        //  --> 3 variables 3 possible assignment = 3^2*3=18 possibility
//
//        // final result can then be only 6*18*18=1944 options
//    }
}