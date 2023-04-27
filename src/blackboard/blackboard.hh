//
// Created by perso on 27/02/23.
//

#ifndef FABKO_BLACKBOARD_H
#define FABKO_BLACKBOARD_H

#include <concepts>
#include <optional>
#include <string>
#include <vector>

namespace fabko {

struct proposition {
  std::string id;
};

struct blackboard_data {
  std::string id;
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
      { a.instantiate_black_board(std::string{}) } -> std::same_as<blackboard_data>;
      { a.is_request_ready(std::string{}) } -> std::same_as<std::optional<std::vector<proposition>>>;
      { a.commit_decision(std::string{}) } -> std::same_as<decision_status>;
    };

/**
 *
 * @tparam BoardCommunication
 */
template<class BoardCommunication>
  requires c_board_com<BoardCommunication>
class blackboard {
};

}// namespace fabko

#endif//FABKO_BLACKBOARD_H
