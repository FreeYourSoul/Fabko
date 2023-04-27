//
// Created by perso on 27/02/23.
//

#ifndef FABKO_BLACKBOARD_H
#define FABKO_BLACKBOARD_H

#include <concepts>
#include <future>
#include <optional>
#include <string>
#include <vector>

#include "boardcom.hh"

namespace fabko {

struct blackboard_data {
  std::string id;
  com::request_propositions propositions;
};

/**
 *
 * @tparam BoardCommunication
 */
class blackboard {

private:
  struct blackboard_impl;

public:
  ~blackboard();

  template<com::c_board_com BoardCommunication>
  explicit blackboard(BoardCommunication bc);

  [[nodiscard]] std::future<com::request_propositions> request_propositions(const std::string& request);
  [[nodiscard]] std::future<com::decision_status> submit_decision(const std::string& decision);

private:
  std::unique_ptr<blackboard_impl> _pimpl;
};

}// namespace fabko

#endif//FABKO_BLACKBOARD_H
