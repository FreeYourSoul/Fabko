// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 10/11/2023. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <catch2/catch_test_macros.hpp>

#include <protocol/acl.hh>

struct fake_content {

    unsigned id{};
    std::string name;

    friend void to_json(nlohmann::json& serializer, const fake_content& obj) {
        serializer["id"]   = obj.id;
        serializer["name"] = obj.name;
    }

    friend void from_json(const nlohmann::json& serializer, fake_content& obj) {
        serializer.at("id").get_to(obj.id);
        serializer.at("name").get_to(obj.name);
    }
};

#include <fmt/format.h>

TEST_CASE("ACL serialization / deserialization") {

    fake_content content{
        .id   = 4242,
        .name = "ChocoboOfDoom"};

    fabko::acl::message msg{
        .type      = fabko::acl::message_type::inform,
        .sender    = "myself",
        .receivers = {"choco1", "choco2"},
        .content   = fabko::acl::to_binary(content),
        .ontology  = "race"
    };

    fmt::print("print it :: {} \n", msg.content_as_string());

    fabko::acl::serializer serializer = msg;
    const std::string equivalent =
        R"json({"content":[162,98,105,100,25,16,146,100,110,97,109,101,109,67,104,111,99,111,98,111,79,102,68,111,111,109],"ontology":"race","receivers":["choco1","choco2"],"sender":"myself","type":7})json";

    SECTION("serialize in json") {

        std::string json = serializer.dump();

        CHECK(json == equivalent);

        std::string other_json = fabko::acl::to_json(msg);

        CHECK(json == other_json);

    } // End section : serialize in json

    SECTION("deserialize from json") {
        // explicit deserialization
        fabko::acl::message new_msg;
        fabko::acl::deserialize(equivalent, new_msg);

        auto dump         = serializer.dump();
        auto dump_new_msg = fabko::acl::to_json(new_msg);

        CHECK(dump == dump_new_msg);
        // ensure result is equal to initial value
        CHECK(dump_new_msg == equivalent);

    } // End section : deserialize from json

    SECTION("de/serialize in binary") {

        auto binary = fabko::acl::to_binary(msg);

        CHECK(!binary.empty());

        fabko::acl::message new_msg;
        fabko::acl::deserialize(binary, new_msg);

        CHECK(fabko::acl::to_json(new_msg) == equivalent);

    } // End section : de/serialize in binary

} // End TestCase : ACL serialization / deserialization


#include <iostream>
#include <optional>

#include <agent.hh>
#include <protocol/acl.hh>

class my_agent : public fabko::agent {

    auto run_logic(std::optional<fabko::acl::message> msg) {
        std::cout << lol << "\n";

        // infer and fill up priority list of actions
        // sort the list of action to do by priority. Ongoing action has a bigger weight as it is currently being done and most likely
        // impact multiple agent.

        std::vector<fabko::acl::message> communications_to_do;

        // 1.
        // Check if a message has been received
        if (msg.has_value()) {

            // fill the message and try to interpret it,
            // if it can't be interpreted. sent a not_understood to the callee.

            // if it's an action request
            // -- check if you want to do it depending on whatever criteria (what you are currently doing, is it in your best interest)
            //  - add in the action queue the
        }
        // 1.
        // what am I currently doing ?

        // 2. Am I awaiting

        // 3.
        // Return the messages that should be sent after current message processing (if any)
        return communications_to_do;
    }

  public:
    virtual ~my_agent() = default;
    my_agent() : fabko::agent(
        [](fabko::agent& a, std::optional<fabko::acl::message> input)
            -> std::vector<fabko::acl::message> {
            auto self = static_cast<my_agent&>(a);
            return self.run_logic(input);
        }) {}

    int lol{42};
};

TEST_CASE("dada") {
    fabko::agent_runner r;
    r.add_agent(my_agent{});
    r.run();
}