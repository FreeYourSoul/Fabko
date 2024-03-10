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

#include <filesystem>
#include <fstream>

#include "common/string_utils.hh"
#include <common/logging.hh>
#include <logic/sat/dimacs_compiler.hh>

namespace {

void compare_dimacs_files(const std::filesystem::path& f1, const std::filesystem::path& f2) {

  // validate existence of the two file
  CHECK(std::filesystem::exists(f1));
  CHECK(std::filesystem::exists(f2));

  std::string string1, string2;
  std::fstream file1(f1), file2(f2);

  for (int line = 0; !file1.eof(); ++line) {
    std::getline(file1, string1);
    std::getline(file2, string2);

    string1 = fabko::trim_string(string1);
    string2 = fabko::trim_string(string2);

    const bool compare_line = string1 == string2;
    if (!compare_line) {
      fabko::log_error(
          "difference line {}\n"
          " -- file1::{} -- '{}'\n"
          " -- file2::{} -- '{}'\n",
          line,
          std::string(f1), string1,
          std::string(f2), string2);
    }
    CHECK(compare_line);
  }

  // validate that both files are finished to be compared
  CHECK(file1.eof());
  CHECK(file2.eof());
}

} // namespace

TEST_CASE("test sat::solver::from_dimacs") {
  fabko::init_logger();

  // p cnf 1 1
  //  1 0
  // -1 0
  auto testing_dimacs_path = std::filesystem::path(TEST_ASSETS_PATH) / "test.dimacs";
  fabko::sat::solver s     = fabko::sat::from_dimacs(testing_dimacs_path);

  s.solve();
  CHECK(s.solving_status() == fabko::sat::solver_status::UNSAT);

  auto result_destination = std::filesystem::temp_directory_path() / "result.dimacs";
  to_dimacs(s, result_destination);

  compare_dimacs_files(testing_dimacs_path, result_destination);
}