// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <memory>
#include <concepts>
#include <future>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "acl.hh"
#include "fap_request.hh"

namespace fabko::agent_protocol {

struct proposition {
    std::string id;
};

struct propositions {
    std::optional<std::vector<proposition>> props;
    status st{status::awaiting};
};

enum class decision_status {
    SUCCESS,
    RETRY,
    CANCELLED,
};

/**
 *
 * @tparam T
 */
template<typename T>
concept c_communicate =
    requires(T a) {
        { T::instantiate_blackboard(acl::message{}) } -> std::convertible_to<std::string>;
        { a.propositions(acl::message{}) } -> std::convertible_to<std::future<propositions>>;
        { a.commit_decision(acl::message{}) } -> std::convertible_to<decision_status>;
    } && std::movable<T>;

namespace p2p {
class agent_com {
  public:
    static std::string instantiate_blackboard(const acl::message&);
    std::future<propositions> request_propositions(const acl::message&);
    decision_status commit_decision(const acl::message&);
};

} // namespace p2p

namespace local {
class agent_com {
    struct impl;

  public:
    ~agent_com();
    agent_com();

    static std::string instantiate_blackboard(const acl::message&);
    std::future<propositions> request_propositions(const acl::message&);
    decision_status commit_decision(const acl::message&);

  private:
    std::unique_ptr<impl> _impl;
 };
} // namespace local

using agent_com = std::variant<local::agent_com, p2p::agent_com>;

} // namespace fabko::agent_protocol
