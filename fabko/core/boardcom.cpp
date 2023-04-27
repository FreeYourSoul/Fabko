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

#include "boardcom.hh"

namespace fabko {

std::string com::p2p::instantiate_black_board(const std::string&) {
  return {};
}

std::string com::online::instantiate_black_board(const std::string&) {
  return {};
}

std::future<com::propositions> com::online::request_propositions(const std::string&) {
  return {};
}

com::decision_status com::online::commit_decision(const std::string&) {
  return com::decision_status::RETRY;
}

}
