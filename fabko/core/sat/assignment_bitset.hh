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

#pragma once

#include <algorithm>
#include <bitset>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <vector>

#include <fmt/format.h>

#include <common/exception.hh>
#include <sat/solver.hh>

namespace sr = std::ranges;

namespace fabko {

/**
 * Store the state of the cur_assignment of each variable in a SAT solver.
 *
 * The storage is internally handled as a vector of static bitset of size `ChunkSize`
 *
 * @tparam ChunkSize size of each static bitset composing the cur_assignment bitset
 */
template<int ChunkSize = 128>
class assignment_bitset {

public:
  /**
   * Reserve enough space for every currently watched variable cur_assignment to be stored.
   * @note If the total number of variable exceed the current amount of bitset chunk. New
   *    chunk are reserved.
   *
   * @param number_variable number of new variable to prepare for cur_assignment
   */
  void reserve_new_variable(std::size_t number_variable) {
    _total_variable_number += number_variable;

    fabko_assert(_total_variable_number > 0);

    const std::size_t chunks = ((_total_variable_number - 1) / ChunkSize) + 1;
    _unassigned.resize(chunks);
    _assigned.resize(chunks);

    fabko_assert(_unassigned.capacity() == _assigned.capacity());
  }

  /**
   * Check if the variable is assigned or not. Should be used before the usage of is_negated or is_true
   *
   * @param v variable to check
   * @return true if the variable is assigned, false otherwise
   */
  [[nodiscard]] bool is_assigned(sat::variable v) const {
    const auto [index_chunk, index_bitset] = locate(v);
    return _unassigned[index_chunk][index_bitset];
  }

  /**
   * Check if the variable is assigned to false or not
   *
   * @note It is important to validate if the variable is assigned (with ``is_assigned``) before using this function.
   *    If the variable is not assigned it will be visible as false by this function.
   *    The best usage of this method is if you have knowledge that the variable is already assigned (to avoid an necessary check)
   *
   * @param v variable to check
   * @return true if variable assigned to false, false otherwise (could also mean non-assigned if is_assigned is not properly called)
   */
  [[nodiscard]] bool is_negated(sat::variable v) const {
    const auto [index_chunk, index_bitset] = locate(v);
    return !_assigned[index_chunk][index_bitset];
  }

  /**
   * Check if the variable is assigned to true or not
   *
   * @note It is important to validate if the variable is assigned (with ``is_assigned``) before using this function.
   *    If the variable is not assigned it will be visible as false by this function.
   *    The best usage of this method is if you have knowledge that the variable is already assigned (to avoid an necessary check)
   *
   * @param v variable to check
   * @return true if variable assigned to true, false otherwise (could also mean non-assigned if is_assigned is not properly called)
   */
  [[nodiscard]] bool is_true(sat::variable v) const {
    const auto [index_chunk, index_bitset] = locate(v);
    return _assigned[index_chunk][index_bitset];
  }

  /**
   * Check the cur_assignment of the variable.
   *
   * @param v variable to check the cur_assignment on
   * @return std::nullopt if the variable is not assigned, true or false if assigned (depending on cur_assignment)
   */
  [[nodiscard]] std::optional<bool> check_assignment(sat::variable v) const {
    const auto [index_chunk, index_bitset] = locate(v);

    if (!_unassigned[index_chunk][index_bitset]) {
      return std::nullopt;
    }
    return _assigned[index_chunk][index_bitset];
  }

  /**
   * Assign a variable to the provided boolean state
   *
   * @param v id of the variable to assign
   * @param assign boolean value to assign to the provided variable
   */
  void assign_variable(sat::variable v, bool assign) {
    const auto [index_chunk, index_bitset] = locate(v);

    _unassigned[index_chunk].set(index_bitset);
    _assigned[index_chunk].set(index_bitset, assign);
  }

  /**
   * @param v id of the variable to unassign
   */
  void unassign_variable(sat::variable v) {
    const auto [index_chunk, index_bitset] = locate(v);
    _unassigned[index_chunk].reset(index_bitset);
  }

  /**
   * @return true if all variable has been assigned properly, false otherwise
   */
  [[nodiscard]] bool all_assigned() const {
    return sr::all_of(_unassigned, [](const auto& bs) { return bs.all(); });
  }

  /**
   * @return number of variable assigned
   */
  [[nodiscard]] std::size_t number_assigned() const {
    const int chunks_fully_assigned = sr::count(_unassigned, [](const auto& bs) { return bs.all(); });
    const int lasting_chunks = [this, &chunks_fully_assigned]() -> int {
      auto last_chunk = _unassigned.get(chunks_fully_assigned);
      if (!last_chunk.has_value()) {
        return 0;
      }
      return last_chunk.value().count();
    }();

    return chunks_fully_assigned * ChunkSize + lasting_chunks;
  }

  [[nodiscard]] std::size_t nb_vars() const { return _total_variable_number; }
  [[nodiscard]] std::size_t nb_chunks() const { return _unassigned.size(); }

  [[nodiscard]] constexpr std::size_t chunk_size() const { return ChunkSize; }

private:
  /**
   * Locate the variable in the bitset
   *
   * @note This function is used internally in order to retrieve the proper bit that set
   *  the cur_assignment of a variable.
   *
   * @param v variable to locate in the bitset
   * @return a tuple containing the chunk index and the bitset index
   */
  [[nodiscard]] std::pair<std::size_t, std::size_t> locate(sat::variable v) const {
    auto var = static_cast<std::size_t>(v - 1);// variable starts at 1 (bitsets start at 0). normalization
    fabko_assert(
        var < _total_variable_number,
        fmt::format("cannot assign variable {} < total number of variable {} ", var, _total_variable_number));

    const std::size_t index_chunk = var / ChunkSize;
    const std::size_t index_bitset = var % ChunkSize;

    return {index_chunk, index_bitset};
  }

private:
  // chunking the static bitset into dynamic

  //! unassigned :: 0 = unassigned variable :: 1 = assigned value
  std::vector<std::bitset<ChunkSize>> _unassigned{};

  //! assigned   :: 0 = false cur_assignment to variable :: 1 = true cur_assignment to variable
  std::vector<std::bitset<ChunkSize>> _assigned{};

  std::size_t _total_variable_number{0};
};

}// namespace fabko