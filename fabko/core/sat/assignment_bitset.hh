/**
 *
 * Copyright (C) 2023 INAIT.SA - - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 *
 * Proprietary and confidential
 *
 **/

#pragma once

#include <map>

namespace fil {

template<typename T = int>
class boundary_map {
public:
  [[nodiscard]] auto get(int index) const {
    return _map.lower_bound(index);
  }

  void insert(int index, T&& element) {
    auto it = get(index);
    if (it == _map.end()) {
      _map[index] = std::forward<T>(element);
    } else if (element != it->second) {
      _map[it->first + 1] = std::forward<T>(element);
    } else {
      _map.erase(it);
      _map[it->first] = std::forward<T>(element);
    }
  }

  void insert(int index, const T& element) {
    T elem = element;
    insert(index, std::move(elem));
  }

  [[nodiscard]] auto end() const {
    return _map.end();
  }

private:
  std::map<int, T> _map;

};

namespace fabko {

template <int ChunkSize = 128>
class assignment_bitset {
public:


private:
  boundary_map<std::bitset<ChunkSize>> _unassigned{};
  boundary_map<std::bitset<ChunkSize>> _assigned{};

  /**
   * Cursor of assignment.
   * Index into the assignement biteset
   */
  std::size_t _cursor_assign{0};

  std::size_t _total_assignment{0};

};

}// namespace fabko