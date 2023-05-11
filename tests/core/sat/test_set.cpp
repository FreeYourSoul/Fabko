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

#include "logic/formula.hh"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("test_formula :: basic test on set") {

    SECTION("Simple setup set 3 splices") {
        auto line = fabko::logic::set({"A", "B", "C"});
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
            line.splice(0, 1).add_and_constraint("B");
            // BAC
            // BCA
            // only (3 - 1)^1*1 = 2 options -- -1 because one variable is fixed
        }
    }

    SECTION("Splice fails") {
        auto line = fabko::logic::set({"1", "2", "3"});

        // end of splice to large
        CHECK_THROWS(line.splice(0, 4));
        // beginning of splice impossible
        CHECK_THROWS(line.splice(4, 5));
        // first value bigger than second
        CHECK_THROWS(line.splice(2, 0));
    }

    SECTION("9 value set 3 splices with constraint") {

        auto line = fabko::logic::set(9);

        auto view_set_1 = line.splice(0, 3);
        view_set_1.add_or_constraint({"2", "4", "6", "8"});

        auto view_set_2 = line.splice(3, 6);
        view_set_1.add_and_constraint({"2"});
        view_set_1.add_or_constraint({"2", "1", "3", "5"});

        auto view_set_3 = line.splice(6, 9);

        // all those options are per-mutable. And thus x^2*y options is possible
        // with x the number of variable and y the number of possible assignment

        // `first` -> has to be 4 6 8 as `second` must contain 2
        //  --> 3 variables 1 possible assignment = 3^2*1=6

        // assignment second can be: 2 1 3 5 with 2 mandatory
        // 2 1 3
        // 2 1 5
        // 2 3 5
        //  --> 3 variables 3 possible assignment = 3^2*3=18 possibility

        // assignment `third` can be : 1 3 5  (but not more that one of 1 3 5 otherwise
        // `second` is impossible) and thus MUST contain 7 9
        // 1 7 9
        // 3 7 9
        // 5 7 9
        //  --> 3 variables 3 possible assignment = 3^2*3=18 possibility

        // final result can then be only 6*18*18=1944 options
    }
}