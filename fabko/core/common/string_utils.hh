// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 01/05/23. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <numeric>
#include <algorithm>
#include <string>
#include <vector>

namespace fabko {

template<typename Handler>
void split_string(std::string_view input, std::string_view separator, Handler&& handler, int limitation = -1) {

  std::size_t pos_start = 0;
  std::size_t pos_end;

  do {
    if (limitation != -1 && --limitation <= 0) {
      pos_end = std::string::npos;
    } else {
      pos_end = input.find(separator, pos_start);
    }
    std::forward<Handler>(handler)(input.substr(pos_start, pos_end - pos_start));
    pos_start = pos_end + separator.size();
  } while (pos_end != std::string::npos);
}

template<typename Handler>
void split_string(std::string_view input, const std::vector<std::string_view>& separators, Handler&& handler, int limitation = -1) {

  std::size_t pos_start = 0;
  std::size_t pos_end;
  std::vector<std::size_t> tmp;

  tmp.resize(separators.size());
  do {
    for (std::uint32_t i = 0; i < separators.size(); ++i) {
      auto v = input.find(separators.at(i), pos_start);
      tmp[i] = (v == std::string::npos) ? input.size() : v;
    }
    auto index = static_cast<std::size_t>(
        std::distance(tmp.begin(), std::min_element(tmp.begin(), tmp.end())));

    if (limitation != -1 && --limitation <= 0) {
      pos_end = input.size();
    } else {
      pos_end = tmp[index];
    }
    std::forward<Handler>(handler)(input.substr(pos_start, pos_end - pos_start));
    pos_start = pos_end + separators[index].size();

  } while (pos_end < input.size());
}

constexpr auto is_whitespace = [](char c) { return std::isspace(c); };

static std::string_view trim_string(std::string_view to_trim) {

  auto begin_it = std::find_if_not(to_trim.begin(), to_trim.end(), is_whitespace);
  auto end_it = std::find_if_not(to_trim.rbegin(), to_trim.rend(), is_whitespace);

  return {begin_it, end_it.base()};
}

static std::string join(const std::vector<std::string>& to_join, const std::string& separator = "") {
  auto size = separator.size() * to_join.size()
            + std::accumulate(to_join.begin(), to_join.end(), 0uz, [](auto v, const auto& j) { return v + j.size(); });

  std::string result;
  result.reserve(size);

  bool first = true;
  for (const auto& j : to_join) {
    if (first) {
      first = false;
    } else {
      result.append(separator);
    }
    result.append(j);
  }
  return result;
}


} // namespace fabko
