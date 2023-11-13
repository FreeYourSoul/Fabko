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

#include <acl.hh>

struct fake_content {

    unsigned id;
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

TEST_CASE("ACL serialization / deserialization") {

    fake_content content{
        .id   = 4242,
        .name = "ChocoboOfDoom"};

    fabko::acl::message<fake_content> msg{
        .type      = fabko::acl::message_type::inform,
        .sender    = "myself",
        .receivers = {"choco1", "choco2"},
        .content   = content,
        .ontology  = "race"
    };

    fabko::acl::serializer serializer = msg;
    const std::string equivalent =
        R"json({"content":{"id":4242,"name":"ChocoboOfDoom"},"ontology":"race","receivers":["choco1","choco2"],"sender":"myself","type":7})json";

    SECTION("serialize in json") {

        std::string json = serializer.dump();

        CHECK(json == equivalent);

        std::string other_json = fabko::acl::to_json(msg);

        CHECK(json == other_json);

    } // End section : serialize in json

    SECTION("deserialize from json") {
        // explicit deserialization
        fabko::acl::message<fake_content> new_msg;
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

        fabko::acl::message<fake_content> new_msg;
        fabko::acl::deserialize(binary, new_msg);

        CHECK(fabko::acl::to_json(new_msg) == equivalent);

    } // End section : de/serialize in binary

} // End TestCase : ACL serialization / deserialization