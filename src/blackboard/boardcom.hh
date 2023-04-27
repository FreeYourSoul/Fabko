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
