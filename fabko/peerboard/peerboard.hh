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

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <blackboard.hh>

namespace fabko {

enum acceptance_requirement {
  NONE,
  REQUEST_AGENT_VALIDATION,
  QUERY_AGENT_VALIDATION,
  BOTH_AGENT_VALIDATION,
};

using capability_meta = std::variant<std::string, int, double>;

struct capability {

  std::string id;
//  std::string description;

  /** result that would be generated out of the activity. */
  capability_meta result;
  /**
   * metadata of the activity : those could be used in order to provide additional
   * information over the capability as metrics which could be used in rules.
   */
  std::unordered_map<std::string, capability_meta> metadata;

   /** specific capabilities required to be done before this capability is executed. */
  std::vector<std::string> cap_deps;

  /**
   * @brief Acceptance is used in order to determine if the provider of this capability
   * validation requirement are.
   *
   * In other words, what is the action that the provider of the capability has to
   * do in order to validate that the action will be done.
   */
  acceptance_requirement accept = acceptance_requirement::NONE;
};


class peerboard {
  struct pb_impl;

public:
  explicit peerboard(std::string data_store_location);
  ~peerboard();

  void register_capability(capability to_add);
  void unregister_capability(const std::string& id);

  std::unique_ptr<blackboard> make_blackboard(const std::string& requested_action);

private:
  std::unique_ptr<pb_impl> _pimpl;

};

}
