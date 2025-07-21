// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 18.07.25. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include "compiler/backend/sat/solver.hh"
#include "compiler/backend/sat/solver_context.hh"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("test watcher", "sat") {
    fabko::compiler::sat::Vars_Soa vars;
    fabko::compiler::sat::Clauses_Soa clauses;

    SECTION("test that if all vars are assigned :: watcher is empty") {
        fabko::compiler::sat::Literal l1 {1};
        fabko::compiler::sat::Literal l2 {2};
        fabko::compiler::sat::Literal l3 {3};

        auto var_id1 = vars.insert(l1, fabko::compiler::sat::assignment::off, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});
        auto var_id2 = vars.insert(l2, fabko::compiler::sat::assignment::off, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});
        auto var_id3 = vars.insert(l3, fabko::compiler::sat::assignment::off, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});

        fabko::compiler::sat::Clause clause {
            {l1,      l2,      l3     },
            {var_id1, var_id2, var_id3}
        };
        fabko::compiler::sat::Clause_Watcher watcher {vars, clause};

        CHECK(watcher.size() == 0);
    }

    SECTION("test that if a single variable is not assigned :: watcher is of size 1") {
        fabko::compiler::sat::Literal l1 {1};
        fabko::compiler::sat::Literal l2 {2};
        fabko::compiler::sat::Literal l3 {3};

        auto var_id1 = vars.insert(l1, fabko::compiler::sat::assignment::off, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});
        auto var_id2 = vars.insert(l2, fabko::compiler::sat::assignment::not_assigned, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});
        auto var_id3 = vars.insert(l3, fabko::compiler::sat::assignment::off, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});

        fabko::compiler::sat::Clause clause {
            {l1,      l2,      l3     },
            {var_id1, var_id2, var_id3}
        };
        fabko::compiler::sat::Clause_Watcher watcher {vars, clause};

        CHECK(watcher.size() == 1);
        CHECK(watcher.get_watched().size() == 1);
        CHECK(watcher.get_watched()[0].offset == var_id2.offset);
    }

    SECTION("test that if two variables is not assigned :: watcher is of size 2") {
        fabko::compiler::sat::Literal l1 {1};
        fabko::compiler::sat::Literal l2 {2};
        fabko::compiler::sat::Literal l3 {3};

        auto var_id1 = vars.insert(l1, fabko::compiler::sat::assignment::off, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});
        auto var_id2 = vars.insert(l2, fabko::compiler::sat::assignment::not_assigned, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});
        auto var_id3 = vars.insert(l3, fabko::compiler::sat::assignment::not_assigned, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});

        fabko::compiler::sat::Clause clause {
            {l1,      l2,      l3     },
            {var_id1, var_id2, var_id3}
        };
        fabko::compiler::sat::Clause_Watcher watcher {vars, clause};
        const auto size_watcher = watcher.size();

        CHECK(size_watcher == 2);
        CHECK(watcher.get_watched().size() == 2);
        CHECK(watcher.get_watched()[0].offset == var_id2.offset);
        CHECK(watcher.get_watched()[1].offset == var_id3.offset);
    }

    SECTION("test that if three variables is not assigned :: watcher is of size 2") {
        fabko::compiler::sat::Literal l1 {1};
        fabko::compiler::sat::Literal l2 {2};
        fabko::compiler::sat::Literal l3 {3};

        auto var_id1 = vars.insert(l1, fabko::compiler::sat::assignment::not_assigned, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});
        auto var_id2 = vars.insert(l2, fabko::compiler::sat::assignment::not_assigned, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});
        auto var_id3 = vars.insert(l3, fabko::compiler::sat::assignment::not_assigned, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});

        fabko::compiler::sat::Clause clause {
            {l1,      l2,      l3     },
            {var_id1, var_id2, var_id3}
        };
        fabko::compiler::sat::Clause_Watcher watcher {vars, clause};
        const auto size_watcher = watcher.size();

        CHECK(size_watcher == 2);
        CHECK(watcher.get_watched().size() == 2);
        CHECK(watcher.get_watched()[0].offset == var_id1.offset);
        CHECK(watcher.get_watched()[1].offset == var_id2.offset);

        SECTION("test replace :: check that replacing an element that is not watched does not change the watcher") {

            watcher.replace(vars, clause, var_id3);

            CHECK(watcher.size() == 2);
            CHECK(watcher.get_watched().size() == 2);
            CHECK(watcher.get_watched()[0].offset == var_id1.offset);
            CHECK(watcher.get_watched()[1].offset == var_id2.offset);
        }

        SECTION("test replace :: check that if replace an element watched but the element is still un-assigned, it is taken again (no change in watcher)") {

            watcher.replace(vars, clause, var_id1); // element exists but has not assigned still, so it's retrieved back

            CHECK(watcher.size() == 2);
            CHECK(watcher.get_watched().size() == 2);
            CHECK(watcher.get_watched()[0].offset == var_id1.offset);
            CHECK(watcher.get_watched()[1].offset == var_id2.offset);
        }

        SECTION("test replace :: check that if replace an element that has been assigned take another element") {

            get<fabko::compiler::sat::soa_assignment>(vars[var_id1]) = fabko::compiler::sat::assignment::on; // assign the first variable
            // element exists but and has been assigned, new element found
            watcher.replace(vars, clause, var_id1);

            CHECK(watcher.size() == 2);
            CHECK(watcher.get_watched().size() == 2);
            CHECK(watcher.get_watched()[0].offset == var_id3.offset);
            CHECK(watcher.get_watched()[1].offset == var_id2.offset);
        }

        SECTION("test replace :: test that if nothing left to be assigned, nothing get assigned ") {
            get<fabko::compiler::sat::soa_assignment>(vars[var_id1]) = fabko::compiler::sat::assignment::on; // assign the first variable
            get<fabko::compiler::sat::soa_assignment>(vars[var_id3]) = fabko::compiler::sat::assignment::on; // assign the last variable

            watcher.replace(vars, clause, var_id1);

            CHECK(watcher.size() == 1);
            CHECK(watcher.get_watched().size() == 1);
            CHECK(watcher.get_watched()[0].offset == var_id2.offset);
        }
    }

    SECTION("test single element in clause : size 1 if not assigned") {
        fabko::compiler::sat::Literal l1 {1};

        auto var_id1 = vars.insert(l1, fabko::compiler::sat::assignment::not_assigned, fabko::compiler::sat::Assignment_Context {}, fabko::compiler::Metadata {});

        fabko::compiler::sat::Clause clause {{l1}, {var_id1}};
        fabko::compiler::sat::Clause_Watcher watcher {vars, clause};
        const auto size_watcher = watcher.size();

        CHECK(size_watcher == 1);
        CHECK(watcher.get_watched().size() == 1);
        CHECK(watcher.get_watched()[0].offset == var_id1.offset);
    }
}