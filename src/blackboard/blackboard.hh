//
// Created by perso on 27/02/23.
//

#ifndef FABKO_BLACKBOARD_H
#define FABKO_BLACKBOARD_H

#include <concepts>
#include <future>
#include <optional>
#include <string>
#include <vector>

namespace fabko {

struct proposition {
  std::string id;
};

using request_propositions = std::optional<std::vector<proposition>>;

struct blackboard_data {
  std::string id;
  request_propositions propositions;
};

enum class decision_status {
  SUCCESS,
  RETRY,
  CANCELLED,
};

/**
 *
 * @tparam T
 */
template<typename T>
concept c_board_com =
    requires(T a) {
      { a.instantiate_black_board(std::string{}) } -> std::same_as<std::string>;
      { a.request_propositions(std::string{}) } -> std::same_as<std::future<request_propositions>>;
      { a.commit_decision(std::string{}) } -> std::same_as<decision_status>;
    } && std::movable<T>;

namespace details {
class blackboard_impl {
  virtual blackboard_data instantiate_black_board() = 0;
};

template<c_board_com BoardCommunication>
class blackboard_impl_store : public blackboard_impl {
public:
  BoardCommunication _bc;
  blackboard_data _data{};

  blackboard_data instantiate_black_board() override {
    _data.id = _bc->instantiate_black_board();
    return _data;
  }
};

}// namespace details

/**
 *
 * @tparam BoardCommunication
 */
class blackboard {

private:
public:
  template<c_board_com BoardCommunication>
  explicit blackboard(BoardCommunication&& bc)
      : _pimpl(std::make_unique<details::blackboard_impl_store<BoardCommunication>>(std::forward<BoardCommunication>(bc))) {}

  [[nodiscard]] std::future<request_propositions> request_propositions(const std::string& request);
  [[nodiscard]] std::future<decision_status> submit_decision(const std::string& decision);

private:
  std::unique_ptr<details::blackboard_impl> _pimpl;
};

}// namespace fabko

#endif//FABKO_BLACKBOARD_H
