// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 01/05/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <catch2/catch_test_macros.hpp>

#include <common/string_utils.hh>

TEST_CASE("algorithm_testcase string") {

    SECTION("basic trim test") {

        REQUIRE(fabko::trim_string("    dada    ") == "dada");
        REQUIRE(fabko::trim_string("    dada") == "dada");
        REQUIRE(fabko::trim_string("dada ") == "dada");
        REQUIRE(fabko::trim_string(" dada ") == "dada");

        REQUIRE(fabko::trim_string("\t\tdada\t\t") == "dada");
        REQUIRE(fabko::trim_string("\tdada\t") == "dada");
        REQUIRE(fabko::trim_string("\tdada") == "dada");
        REQUIRE(fabko::trim_string("dada\t") == "dada");

    } // End section : basic split_string test

    SECTION("basic split_string test") {

        std::uint32_t counter = 0;
        fabko::split_string("this;contains;three;semi-column", ";", [&counter](std::string_view fragment) { ++counter; });
        REQUIRE(4 == counter);

    } // End section : basic split_string test

    SECTION("split_string no occurrence") {

        std::uint32_t counter = 0;
        std::string value;
        fabko::split_string("this", ";", [&counter, &value](std::string_view fragment) { ++counter; value = fragment; });
        REQUIRE(1 == counter);
        REQUIRE("this" == value);

    } // End section : basic split_string test

    SECTION("split_string no occurrence") {

        std::uint32_t counter = 0;
        std::vector<std::string> values;
        fabko::split_string(";", ";", [&counter, &values](std::string_view fragment) { ++counter; values.emplace_back(fragment); });
        REQUIRE(2 == counter);
        REQUIRE(values.at(0).empty());
        REQUIRE(values.at(1).empty());

    } // End section : basic split_string test

    SECTION("basic split_string check fragment") {

        std::vector<std::string> fragments;
        fabko::split_string("this;contains;three;semi-column", ";", [&fragments](std::string_view fragment) { fragments.emplace_back(std::move(fragment)); });
        REQUIRE(4 == fragments.size());
        REQUIRE("this" == fragments.at(0));
        REQUIRE("contains" == fragments.at(1));
        REQUIRE("three" == fragments.at(2));
        REQUIRE("semi-column" == fragments.at(3));

    } // End section : basic split_string test

    SECTION("basic split_string check fragment with limitation") {

        std::vector<std::string> fragments;
        fabko::split_string(
            "this;contains;three;semi-column", ";", [&fragments](std::string_view fragment) { fragments.emplace_back(std::move(fragment)); }, 2);
        REQUIRE(2 == fragments.size());
        REQUIRE("this" == fragments.at(0));
        REQUIRE("contains;three;semi-column" == fragments.at(1));

    } // End section : basic split_string test

    SECTION("basic split_string long separator") {

        std::vector<std::string> fragments;
        fabko::split_string("this[-----]contains[-----]three[-----]semi-column", "[-----]",
                            [&fragments](std::string_view fragment) { fragments.emplace_back(std::move(fragment)); });
        REQUIRE(4 == fragments.size());
        REQUIRE("this" == fragments.at(0));
        REQUIRE("contains" == fragments.at(1));
        REQUIRE("three" == fragments.at(2));
        REQUIRE("semi-column" == fragments.at(3));

    } // End section : basic split_string test

    SECTION("basic split_string multi separator check fragment") {

        std::vector<std::string> fragments;
        fabko::split_string("this;contains:three?semi-column", {"?", ":", ";"}, [&fragments](std::string_view fragment) { fragments.emplace_back(fragment); });
        REQUIRE(4 == fragments.size());
        REQUIRE("this" == fragments.at(0));
        REQUIRE("contains" == fragments.at(1));
        REQUIRE("three" == fragments.at(2));
        REQUIRE("semi-column" == fragments.at(3));

    } // End section : basic split_string test

    SECTION("basic split_string multi separator long separator") {

        std::vector<std::string> fragments;
        fabko::split_string("this//////contains:three[-----]semi-column", {"[-----]", ":", "//////"},
                            [&fragments](std::string_view fragment) { fragments.emplace_back(fragment); });
        REQUIRE(4 == fragments.size());
        REQUIRE("this" == fragments.at(0));
        REQUIRE("contains" == fragments.at(1));
        REQUIRE("three" == fragments.at(2));
        REQUIRE("semi-column" == fragments.at(3));

    } // End section : basic split_string test

    SECTION("basic split_string multi separator long separator with limitation") {

        std::vector<std::string> fragments;
        fabko::split_string(
            "this//////contains:three[-----]semi-column", {"[-----]", ":", "//////"},
            [&fragments](std::string_view fragment) { fragments.emplace_back(fragment); }, 3);
        REQUIRE(3 == fragments.size());
        REQUIRE("this" == fragments.at(0));
        REQUIRE("contains" == fragments.at(1));
        REQUIRE("three[-----]semi-column" == fragments.at(2));

    } // End section : basic split_string test

    SECTION("string join") {

        std::vector<std::string> strings{"this", "is", "a", "string", "split"};

        CHECK("thisisastringsplit" == fabko::join(strings));

        CHECK("this is a string split" == fabko::join(strings, " "));

        CHECK("this | | is | | a | | string | | split" == fabko::join(strings, " | | "));

    } // End section : string join

} // end testcase : algorithm_testcase string