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

#include <optional>
#include <unordered_map>
#include <string>
#include <vector>

namespace fabko::acl {

// ACL standard is followed in a binary format (and a backward compatibility should be following afterwards on the actual
// textual standard:
// reference to the standard used for the Agent Communication Language (ACL):
//   http://www.fipa.org/specs/fipa00018/OC00018.pdf

enum class message_type : unsigned {
    // The action of accepting a previously submitted proposal to perform an action.
    accept_proposal = 0,

    // The action of agreeing to perform some action, possibly in the future.
    agree = 1,

    // The action of cancelling some previously requested action which has temporal extent (i.e. is not instantaneous).
    cancel = 2,

    // The action of calling for proposals to perform a given action.
    cfp = 3,

    // The sender informs the receiver that a given proposition is true, where the receiver is
    // known to be uncertain about the proposition.
    confirm = 4,

    // The sender informs the receiver that a given proposition is false, where the receiver
    // is known to believe, or believe it likely that, the proposition is true.
    disconfirm = 5,

    // The action of telling another agent that an action was attempted but the attempt failed.
    failure = 6,

    // The sender informs the receiver that a given proposition is true.
    inform = 7,

    // A macro action for the agent of the action to inform the recipient whether a proposition is true
    inform_if = 8,

    not_understood   = 9,
    propose          = 10,
    query_if         = 11, // macro act
    query_ref        = 12,
    refuse           = 13,
    reject_proposal  = 14,
    request          = 15,
    request_when     = 16,
    request_whenever = 17,
    subscribe        = 18,

    // A macro action for sender to inform the receiver the object which corresponds to a definite descriptor (e.g. a name).
    inform_ref       = 19, // macro act
};

constexpr bool is_negotiation(message_type type) {
    return type == message_type::accept_proposal || type == message_type::cfp
        || type == message_type::propose || type == message_type::reject_proposal;
}

constexpr bool is_action(message_type type) {
    return type == message_type::agree || type == message_type::cancel || type == message_type::refuse || type == message_type::request
        || type == message_type::query_ref || type == message_type::request_when || type == message_type::request_whenever;
}

constexpr bool is_info_passing(message_type type) {
    return type == message_type::inform || type == message_type::inform_if || type == message_type::inform_ref
        || type == message_type::failure || type == message_type::confirm || type == message_type::disconfirm;
}

constexpr bool is_query(message_type type) {
    return type == message_type::query_if || type == message_type::query_ref
        || type == message_type::subscribe || type == message_type::refuse;
}

constexpr bool is_error_handling(message_type type) {
    return type == message_type::failure || type == message_type::inform || type == message_type::not_understood;
}

struct acc {
    unsigned id;

    // communication protocol : which can be used in other to define different ways to communicate between the agents
    std::string protocol;

    // reply_by / reply_with is a protocol agnostic way to handle communication between a sender and a receiver.
    // the sender fill the reply_with field in order to have an expectation for the receiver to reply with a certain data.
    // this can be used in order to recognise a specific thread of communication out of a single agent communication channel
    std::optional<std::string> reply_with = std::nullopt;
    std::optional<std::string> reply_by = std::nullopt;

};

template<typename MessageContent>
struct message {
    message_type type;
    std::string sender;
    std::vector<std::string> receivers;

    // can be used as a metadata holder for the message. Depending on the protocol used. The envelope can be a crucial header part
    // of the message or just a metadata bank for the message itself (time sent etc..)
    std::unordered_map<std::string, std::string> envelope;

    MessageContent content;

    // define a domain of application the message is for. An agent receiving a message coming from a not understood ontology should
    // either learn from it if it has something to gain from it. Or discard the message accordingly.
    std::string ontology;

    // conversation information that could be transmitted in order to initiate or continue an Agent Communication Channel (ACC)
    std::optional<acc> communication = std::nullopt;
};

} // namespace fabko::acl
