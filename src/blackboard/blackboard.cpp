//
// Created by perso on 27/02/23.
//

#include <functional>

#include "blackboard.hh"
#include "utils.hh"

namespace fabko {

struct blackboard::blackboard_impl {

  explicit blackboard_impl(com::c_board_com auto&& bc) : bc(std::forward<decltype(bc)>(bc)) {}

  com::board_protocol bc;
  blackboard_data data{};

  blackboard_data instantiate_black_board() {
    data.id = std::visit(
        overloaded{
            [](auto& b) -> std::string {
              return {};
            }},
        bc);
    return data;
  }
};

std::future<com::request_propositions> blackboard::request_propositions(const std::string& request) {
  return {};
}

std::future<com::decision_status> blackboard::submit_decision(const std::string& decision) {
  return {};
}

blackboard::~blackboard() = default;

template<com::c_board_com BoardCommunication>
blackboard::blackboard(BoardCommunication bc) : _pimpl(std::move<BoardCommunication>(bc)) {
}

}// namespace fabko