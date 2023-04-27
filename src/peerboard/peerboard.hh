//
// Created by perso on 27/02/23.
//

#ifndef FABKO_PEERBOARD_H
#define FABKO_PEERBOARD_H

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../blackboard/blackboard.hh"

namespace fabko {

enum acceptance_requirement {
  NONE,
  AGENT_VALIDATION,
};

using capability_meta = std::variant<std::string, int, double>;

struct capability {

  std::string id;
  std::string description;

  //! result that would be generated out of the activity.
  capability_meta result;
  /**
   * metadata of the activity : those could be used in order to provide additional
   * information over the capability as metrics which could be used in rules.
   */
  std::unordered_map<std::string, capability_meta> metadata;

   //! specific capabilities required to be done before this capability is executed.
  std::vector<std::string> cap_deps;

  /**
   * Acceptance is used in order to determine if the provider of this capability
   * validation requirement are.
   * In other words, what is the action that the provider of the capability has to
   * do in order to validate that the action will be done.
   */
  acceptance_requirement accept = acceptance_requirement::NONE;
};

struct peer_rule {

};

class peerboard {

private:
  std::vector<peer_rule> _rule_set;

public:

  std::unique_ptr<blackboard> make_blackboard(const std::string& requested_action);



};

}

#endif //FABKO_PEERBOARD_H
