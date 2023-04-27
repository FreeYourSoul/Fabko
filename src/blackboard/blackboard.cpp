//
// Created by perso on 27/02/23.
//

#include <functional>

#include "blackboard.hh"
#include "utils.hh"

namespace fabko {

struct blackboard::blackboard_impl {

  explicit blackboard_impl(com::c_board_com auto&& bc, blackboard_data data)
      : bc(std::forward<decltype(bc)>(bc)), data(std::move(data)) {}

  blackboard_data instantiate_black_board(const std::string& request) {
    data.id = std::visit(
        overloaded{[&request](auto& b) -> std::string { return b.instantiate_black_board(request); }}, bc);
    return data;
  }

  com::board_protocol bc;
  blackboard_data data;
};

blackboard::~blackboard() = default;

template<com::c_board_com BoardCommunication>
blackboard::blackboard(BoardCommunication bc) : _pimpl(std::move<BoardCommunication>(bc), {}) {
}

std::future<com::request_propositions> blackboard::request_propositions(const std::string& request) {
  return {};
}

std::future<com::decision_status> blackboard::submit_decision(const std::string& decision) {
  return {};
}

}// namespace fabko