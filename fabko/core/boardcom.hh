// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <concepts>
#include <future>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace fabko::com {

struct proposition {
  std::string id;
};

enum class request_process : int {
  INIT = 0,
  IN_PROGRESS = 1,
  DONE = 2,
};

struct propositions {
  std::optional<std::vector<proposition>> props;
  request_process status {request_process::INIT};
};

enum class decision_status {
  SUCCESS,
  RETRY,
  CANCELLED,
};

struct request {
  std::string emitter;
  std::string request;
  std::uint16_t result_requested{1};
};

/**
 *
 * @tparam T
 */
template<typename T>
concept c_board_com =
    requires(T a) {
      { T::make_board(request{}) } -> std::same_as<std::string>;
      { a.propositions(std::string{}) } -> std::same_as<std::future<propositions>>;
      { a.commit_decision(std::string{}) } -> std::same_as<decision_status>;
    } && std::movable<T>;

class p2p {
public:
  static std::string instantiate_black_board(const std::string&);
  std::future<propositions> request_propositions(const std::string&);
  decision_status commit_decision(const std::string&);
};

class online {
public:
  static std::string instantiate_black_board(const std::string&);
  std::future<propositions> request_propositions(const std::string&);
  decision_status commit_decision(const std::string&);
};

using board_protocol = std::variant<p2p, online>;

}// namespace fabko::com
