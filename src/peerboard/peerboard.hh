//
// Created by perso on 27/02/23.
//

#ifndef FABKO_PEERBOARD_H
#define FABKO_PEERBOARD_H

#include <string>
#include <vector>

namespace fabko {

enum acceptance_requirement {
  DIRECT,
  AGENT_VALIDATION,
};

struct capability {

  std::string id;

  std::vector<std::string> dependencies;

  acceptance_requirement accept = acceptance_requirement::DIRECT;
};

class peerboard {

};

}

#endif //FABKO_PEERBOARD_H
