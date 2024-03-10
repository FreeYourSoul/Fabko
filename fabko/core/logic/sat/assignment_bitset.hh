// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
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
#include <logic/sat/solver.hh>

namespace sr = std::ranges;

namespace fabko {

/**
 * Store the state of the cur_assignment of each var in a SAT solver.
 *
 * The storage is internally handled as a vector of static bitset of size `ChunkSize`
 *
 * @tparam ChunkSize size of each static bitset composing the cur_assignment bitset
 */
template<std::size_t ChunkSize = 128>
class assignment_bitset {

public:
  /**
   * Reserve enough space for every currently watched var cur_assignment to be stored.
   * @note If the total number of var exceed the current amount of bitset chunk. New
   *    chunk are reserved.
   *
   * @param number_variable number of new var to prepare for cur_assignment
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
   * Check if the var is assigned or not. Should be used before the usage of is_negated or is_true
   *
   * @param v var to check
   * @return true if the var is assigned, false otherwise
   */
  [[nodiscard]] bool is_assigned(sat::variable v) const {
    const auto [index_chunk, index_bitset] = locate(v);
    return _unassigned[index_chunk][index_bitset];
  }

  /**
   * Check if the var is assigned to false or not
   *
   * @note It is important to validate if the var is assigned (with ``is_assigned``) before using this function.
   *    If the var is not assigned it will be visible as false by this function.
   *    The best usage of this method is if you have knowledge that the var is already assigned (to avoid an necessary check)
   *
   * @param v var to check
   * @return true if var assigned to false, false otherwise (could also mean non-assigned if is_assigned is not properly called)
   */
  [[nodiscard]] bool is_negated(sat::variable v) const {
    const auto [index_chunk, index_bitset] = locate(v);
    return !_assigned[index_chunk][index_bitset];
  }

  /**
   * Check if the var is assigned to true or not
   *
   * @note It is important to validate if the var is assigned (with ``is_assigned``) before using this function.
   *    If the var is not assigned it will be visible as false by this function.
   *    The best usage of this method is if you have knowledge that the var is already assigned (to avoid an necessary check)
   *
   * @param v var to check
   * @return true if var assigned to true, false otherwise (could also mean non-assigned if is_assigned is not properly called)
   */
  [[nodiscard]] bool is_true(sat::variable v) const {
    const auto [index_chunk, index_bitset] = locate(v);
    return _assigned[index_chunk][index_bitset];
  }

  /**
   * Check if the var is assigned or negated depending on the provided checker
   *
   * @note It is important to validate if the var is assigned (with ``is_assigned``) before using this function.
   *    If the var is not assigned it will be visible as false by this function.
   *    The best usage of this method is if you have knowledge that the var is already assigned (to avoid an necessary check)
   *
   * @param v var to check
   * @param check if set to true equivalent to calling is_true(v), if set to false is_negated(v)
   * @return true if var assigned to true, false otherwise (could also mean non-assigned if is_assigned is not properly called)
   */
  [[nodiscard]] bool is(sat::variable v, bool check) const {
    return check ? is_true(v) : is_negated(v);
  }

  /**
   * Check the cur_assignment of the var.
   *
   * @param v var to check the cur_assignment on
   * @return std::nullopt if the var is not assigned, true or false if assigned (depending on cur_assignment)
   */
  [[nodiscard]] std::optional<bool> check_assignment(sat::variable v) const {
    const auto [index_chunk, index_bitset] = locate(v);

    if (!_unassigned[index_chunk][index_bitset]) {
      return std::nullopt;
    }
    return _assigned[index_chunk][index_bitset];
  }

  /**
   * Assign a var to the provided boolean state
   *
   * @param v id of the var to assign
   * @param assign boolean value to assign to the provided var
   */
  void assign_variable(sat::variable v, bool assign) {
    const auto [index_chunk, index_bitset] = locate(v);

    _unassigned[index_chunk].set(index_bitset);
    _assigned[index_chunk].set(index_bitset, assign);
  }

  /**
   * @param v id of the var to unassigned
   */
  void unassign_variable(sat::variable v) {
    const auto [index_chunk, index_bitset] = locate(v);
    _unassigned[index_chunk].reset(index_bitset);
  }

  /**
   * @return true if all var has been assigned properly, false otherwise
   */
  [[nodiscard]] bool all_assigned() const {
    return number_assigned() == nb_vars();
  }

  /**
   * @return number of var assigned
   */
  [[nodiscard]] std::size_t number_assigned() const {
    const auto chunks_fully_assigned = static_cast<std::size_t>(sr::count_if(_unassigned, [](const auto& bs) { return bs.all(); }));
    const auto lasting_chunks        = static_cast<std::size_t>(_unassigned[chunks_fully_assigned].count());

    return chunks_fully_assigned * ChunkSize + lasting_chunks;
  }

  [[nodiscard]] std::size_t nb_vars() const { return _total_variable_number; }
  [[nodiscard]] std::size_t nb_chunks() const { return _unassigned.size(); }

  [[nodiscard]] constexpr std::size_t chunk_size() const { return ChunkSize; }

  /**
   * Conversion of the bitset into a sat::literal vector
   *
   * @param bitset to convert into a sat vector
   * @return vector of literals resulting from the provided bitset
   */
  friend std::vector<sat::literal> to_literals(const assignment_bitset<ChunkSize>& bitset) {
    std::vector<sat::literal> vec;

    vec.reserve(bitset.nb_vars());
    for (sat::variable v : std::ranges::views::iota(sat::variable{1}, bitset.nb_vars() + 1)) {
      vec.emplace_back(v, bitset.is_true(v));
    }
    return vec;
  }

private:
  /**
   * Locate the var in the bitset
   *
   * @note This function is used internally in order to retrieve the proper bit that set
   *  the cur_assignment of a var.
   *
   * @param v var to locate in the bitset
   * @return a tuple containing the chunk index and the bitset index
   */
  [[nodiscard]] std::pair<std::size_t, std::size_t> locate(sat::variable v) const {
    auto var = static_cast<sat::variable>(v - 1); // var starts at 1 (bitsets start at 0). normalization
    fabko_assert(
        var < _total_variable_number,
        fmt::format("cannot locate variable {} < total number of var {} :: retrieved variable indexing is {}", v, _total_variable_number, var));

    const std::size_t index_chunk  = var / ChunkSize;
    const std::size_t index_bitset = var % ChunkSize;

    return {index_chunk, index_bitset};
  }

private:
  // chunking the static bitset into dynamic

  //! unassigned :: 0 = unassigned var :: 1 = assigned value
  std::vector<std::bitset<ChunkSize>> _unassigned{};

  //! assigned   :: 0 = false cur_assignment to variable :: 1 = true cur_assignment to var
  std::vector<std::bitset<ChunkSize>> _assigned{};

  std::size_t _total_variable_number{0};
};

} // namespace fabko