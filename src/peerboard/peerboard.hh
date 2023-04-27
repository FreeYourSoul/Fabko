//
// Created by perso on 27/02/23.
//

#ifndef FABKO_PEERBOARD_H
#define FABKO_PEERBOARD_H

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fabko {

enum acceptance_requirement {
  DIRECT,
  AGENT_VALIDATION,
};

using capability_meta = std::variant<std::string, int, double>;

struct capability {

  std::string id;

  std::unordered_map<std::string, capability_meta> metadata;
  std::vector<std::string> dependencies;

  std::string description;

  acceptance_requirement accept = acceptance_requirement::DIRECT;
};

class peerboard {

};

}

#endif //FABKO_PEERBOARD_H
