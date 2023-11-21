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
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

namespace fabko::acl {

// ACL standard is followed in a binary format (and a backward compatibility should be following afterwards on the actual
// textual standard:
// reference to the standard used for the Agent Communication Language (ACL):
//   http://www.fipa.org/specs/fipa00018/OC00018.pdf

enum class message_type : unsigned {
    //! The action of accepting a previously submitted proposal to perform an action.
    accept_proposal = 0,

    //! The action of agreeing to perform some action, possibly in the future.
    agree = 1,

    //! The action of cancelling some previously requested action which has temporal extent (i.e. is not instantaneous).
    cancel = 2,

    //! The action of calling for proposals to perform a given action.
    cfp = 3,

    //! The sender informs the receiver that a given proposition is true, where the receiver is
    //! known to be uncertain about the proposition.
    confirm = 4,

    //! The sender informs the receiver that a given proposition is false, where the receiver
    //! is known to believe, or believe it likely that, the proposition is true.
    disconfirm = 5,

    //! The action of telling another agent that an action was attempted but the attempt failed.
    failure = 6,

    //! The sender informs the receiver that a given proposition is true.
    inform = 7,

    //! A macro action for the agent of the action to inform the recipient whether a proposition is true
    inform_if = 8,

    //! The sender of the act (e.g. i) informs the receiver (e.g. j) that it perceived that j performed some action, but that
    //! i did not understand what j just did. A particular common case is that i tells j that i did not understand the message
    //! that j has just sent to i.
    not_understood   = 9,

    //! The action of submitting a proposal to perform a certain action, given certain
    //! preconditions.
    propose          = 10,

    //! The action of asking another agent whether or not a given proposition is true.
    query_if         = 11, // macro act

    //! The action of asking another agent for the object referred to by an expression.
    query_ref        = 12,

    //! The action of refusing to perform a given action, and explaining the reason for the refusal.
    refuse           = 13,

    //! The action of rejecting a proposal to perform some action during a negotiation.
    reject_proposal  = 14,

    //! The sender requests the receiver to perform some action.
    //! One important class of uses of the request act is to request the receiver to perform another communicative act.
    request          = 15,

    //! The sender wants the receiver to perform some action when some given proposition becomes true.
    request_when     = 16,

    //! The sender wants the receiver to perform some action as soon as some proposition becomes true and thereafter
    //! each time the proposition becomes true again.
    request_whenever = 17,

    //! The act of requesting a persistent intention to notify the sender of the value of a reference, and to notify again
    //! whenever the object identified by the reference changes.
    subscribe        = 18,

    //! A macro action for sender to inform the receiver the object which corresponds to a definite descriptor (e.g. a name).
    inform_ref = 19, // macro act
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
    unsigned identifier{};

    // communication protocol : which can be used in other to define different ways to communicate between the agents
    std::string protocol;

    // reply_by / reply_with is a protocol agnostic way to handle communication between a sender and a receiver.
    // the sender fill the reply_with field in order to have an expectation for the receiver to reply with a certain data.
    // this can be used in order to recognise a specific thread of communication out of a single agent communication channel
    std::optional<std::string> reply_with = std::nullopt;
    std::optional<std::string> reply_by   = std::nullopt;

    friend void to_json(nlohmann::json& serializer, const acc& obj) {
        serializer["protocol"] = obj.protocol;
        if (obj.reply_with.has_value()) {
            serializer["reply_with"] = obj.reply_with.value();
        }
        if (obj.reply_by.has_value()) {
            serializer["reply_by"] = obj.reply_by.value();
        }
    }

    friend void from_json(const nlohmann::json& serializer, acc& obj) {
        serializer.at("protocol").get_to(obj.protocol);
        if (obj.reply_with.has_value()) {
            obj.reply_with = serializer.at("reply_with").get<std::string>();
        }
        if (obj.reply_by.has_value()) {
            obj.reply_by = serializer.at("reply_by").get<std::string>();
        }
    }
};

struct message {

    message_type type = message_type::not_understood;
    std::string sender;
    std::vector<std::string> receivers;

    // can be used as a metadata holder for the message. Depending on the protocol used. The envelope can be a crucial header part
    // of the message or just a metadata bank for the message itself (time sent etc..)
    std::unordered_map<std::string, std::string> envelope;

    // payload content
    std::string content;

    // define a domain of application the message is for. An agent receiving a message coming from a not understood ontology should
    // either learn from it if it has something to gain from it. Or discard the message accordingly.
    std::string ontology;

    // conversation information that could be transmitted in order to initiate or continue an Agent Communication Channel (ACC)
    std::optional<acc> communication = std::nullopt;

    friend void to_json(nlohmann::json& serializer, const message& obj) {
        serializer["type"]      = obj.type;
        serializer["sender"]    = obj.sender;
        serializer["receivers"] = obj.receivers;
        serializer["content"]   = obj.content;
        serializer["ontology"]  = obj.ontology;
        if (obj.communication.has_value()) {
            serializer["communication"] = obj.communication.value();
        }
    }

    friend void from_json(const nlohmann::json& serializer, message& obj) {
        serializer.at("type").get_to(obj.type);
        serializer.at("sender").get_to(obj.sender);
        serializer.at("receivers").get_to(obj.receivers);
        serializer.at("content").get_to(obj.content);
        serializer.at("ontology").get_to(obj.ontology);
        if (obj.communication.has_value()) {
            obj.communication = serializer.at("communication").get<acc>();
        }
    }
};

using serializer = nlohmann::json;

template<typename ToSerialize>
std::vector<std::uint8_t> to_binary(ToSerialize&& to_serialize) {
    serializer ser = std::forward<ToSerialize>(to_serialize);
    return serializer::to_cbor(ser);
}

template<typename ToSerialize>
std::string to_json(ToSerialize&& to_serialize) {
    serializer ser = std::forward<ToSerialize>(to_serialize);
    return ser.dump();
}

template<typename Serializer, typename ToDeserialize>
    requires std::convertible_to<Serializer, std::string>
          or std::convertible_to<Serializer, std::vector<std::uint8_t>>
          or std::convertible_to<Serializer, serializer>
void deserialize(Serializer&& s, ToDeserialize& into) {
    serializer ser;
    if constexpr (std::convertible_to<Serializer, std::vector<std::uint8_t>>) {
        ser = ser.from_cbor(std::forward<Serializer>(s));
    }
    else if constexpr (std::convertible_to<Serializer, std::string>) {
        ser = serializer::parse(std::forward<Serializer>(s));
    }
    else {
        ser = std::forward<Serializer>(s);
    }
    ser.get_to(into);
}

} // namespace fabko::acl

// optionals has to be explicitly handled through an ADL specialization in order to properly work
namespace nlohmann {
template<typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(json& j, const std::optional<T>& opt) {
        if (opt.has_value()) {
            j = nullptr;
        } else {
            j = opt.value(); // this will call adl_serializer<T>::to_json which will
                             // find the free function to_json in T's namespace!
        }
    }

    static void from_json(const json& j, std::optional<T>& opt) {
        if (j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
};
} // namespace nlohmann