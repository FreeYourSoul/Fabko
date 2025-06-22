// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//
#pragma once

#include <stdexcept>
#include <system_error>

#include <fmt/format.h>

namespace fabko {

namespace except_cat {
struct db : public std::error_category {
  [[nodiscard]] const char* name() const noexcept override { return "fabko::exception::db"; }
  [[nodiscard]] std::string message(int I) const override {
    return fmt::format("Fabko::Database Exception Category : {} : id {}", name(), I);
  }
};

struct fbk : public std::error_category {
  [[nodiscard]] const char* name() const noexcept override { return "fabko::exception"; }
  [[nodiscard]] std::string message(int I) const override {
    return fmt::format("Fabko Exception Category : {} : id {}", name(), I);
  }
};
} // namespace except_cat

class exception : public std::runtime_error {
public:
  exception(const std::error_code& ec, const std::string& what) : std::runtime_error(what), _ec(ec) {}

  [[nodiscard]] int code() const { return _ec.value(); }

private:
  std::error_code _ec;
};

static void fabko_assert(bool assertion, std::error_code ec, const std::string& msg = "") {

#ifndef NDEBUG
    if (!assertion) {
        throw exception(ec, msg);
    }
#else
    (void)assertion;
    (void)ec;
    (void)msg;
#endif
}

static void fabko_assert(bool assertion, const std::string& msg = "") {
#ifndef NDEBUG
    fabko_assert(assertion, {42, except_cat::fbk{}}, msg);
#else
    (void)assertion;
    (void)msg;
#endif
}

} // namespace fabko