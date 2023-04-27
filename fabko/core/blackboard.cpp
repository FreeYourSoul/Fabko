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
blackboard::blackboard(BoardCommunication bc, com::request initial_request)
    : _pimpl(std::move<BoardCommunication>(bc), {.initial_request = std::move(initial_request)}) {
}

com::propositions blackboard::request_propositions(const com::request& request) {
  return {};
}

com::decision_status blackboard::submit_decision(const std::string& decision) {
  return {};
}

}// namespace fabko