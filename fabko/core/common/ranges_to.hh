// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 24/04/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <ranges>
#include <vector>

namespace fabko::ranges {

template<std::ranges::range R>
auto to_vector(R&& r) {
  std::vector<std::ranges::range_value_t<R>> v;

  // if we can get a size, reserve that much
  if constexpr (requires { std::ranges::size(r); }) {
    v.reserve(std::ranges::size(r));
  }

  // push all the elements
  for (auto&& e : r) {
    v.push_back(static_cast<decltype(e)&&>(e));
  }

  return v;
}

} // namespace fabko::ranges
