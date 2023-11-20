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

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "blackboard.hh"

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

} // namespace fabko
