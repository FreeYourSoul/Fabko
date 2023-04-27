//
// Created by perso on 14/03/23.
//

#include "boardcom.hh"

namespace fabko {

std::string com::p2p::instantiate_black_board(const std::string&) {
  return {};
}

std::future<com::request_propositions> com::p2p::request_propositions(const std::string&) {
  return {};
}

com::decision_status com::p2p::commit_decision(const std::string&) {
  return com::decision_status::RETRY;
}

std::string com::online::instantiate_black_board(const std::string&) {
  return {};
}

std::future<com::request_propositions> com::online::request_propositions(const std::string&) {
  return {};
}

com::decision_status com::online::commit_decision(const std::string&) {
  return com::decision_status::RETRY;
}

}
