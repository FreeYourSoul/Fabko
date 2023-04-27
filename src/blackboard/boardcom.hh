//
// Created by perso on 14/03/23.
//

#ifndef FABKO_BOARDCOM_HH
#define FABKO_BOARDCOM_HH

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

using request_propositions = std::optional<std::vector<proposition>>;

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

class p2p {
public:
  std::string instantiate_black_board(const std::string&);
  std::future<request_propositions> request_propositions(const std::string&);
  decision_status commit_decision(const std::string&);
};

class online {
public:
  std::string instantiate_black_board(const std::string&);
  std::future<request_propositions> request_propositions(const std::string&);
  decision_status commit_decision(const std::string&);
};

using board_protocol = std::variant<p2p, online>;

}// namespace fabko::com

#endif//FABKO_BOARDCOM_HH
