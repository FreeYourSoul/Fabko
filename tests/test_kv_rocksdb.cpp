// MIT License
//
// Created by FyS on 28/03/23.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <catch2/catch.hpp>

#include "../fabko/datastore/kv_rocksdb.hh"

TEST_CASE("kv_db_rocksdb_testcase") {

  static_assert(fabko::TransactionalDatabasePolicy<fabko::kv_rocksdb>);

  fabko::kv_rocksdb::initializer_type arg {"tmp.db"};
  fabko::kv_rocksdb_instance rocks_db(arg);

  auto trans = rocks_db.make_transaction();

  trans->set({"Default1", "OKOK"});
  trans->set({"Default2", "Chocobo"});

  SECTION("test get") {
    CHECK("OKOK" == trans->get("Default1"));
    CHECK("Chocobo" == trans->get("Default2"));

    CHECK_THROWS(trans->get("NOT_EXISTING"));
  }

  SECTION("test set") {
    CHECK_THROWS(trans->multi_get({"BABY"}));
    trans->set({"BABY", "CHOCOBO"});
    CHECK("CHOCOBO" == trans->get("BABY"));
  }

  SECTION("test multiset") {
    trans->multi_set({{"BABY", "CHOCOBY"}, {"BABO", "CHOCOBO"}, {"BABA", "CHOCOBA"}});
    CHECK("CHOCOBY" == trans->get("BABY"));
    CHECK("CHOCOBO" == trans->get("BABO"));
    CHECK("CHOCOBA" == trans->get("BABA"));
  }

  SECTION("test multiget") {
    auto res = trans->multi_get({"Default1", "Default2"});

    CHECK(2 == res.size());
    CHECK("Default1" == res.at(0).first);
    CHECK("OKOK" == res.at(0).second);

    CHECK("Default2" == res.at(1).first);
    CHECK("Chocobo" == res.at(1).second);

    CHECK_THROWS(trans->multi_get({"NOT_EXISTING"}));
  }

  SECTION("test get_as") {
    trans->set({"3", "42"});
    trans->set({"4", "16.1337"});

    auto v = trans->get_as<std::uint32_t>("3");
    CHECK(42 == v);

    auto v_double = trans->get_as<double>("4");
    CHECK(16.1337 == v_double);

    auto v_int = trans->get_as<int>("4");
    CHECK(16 == v_int);

    CHECK_THROWS(trans->get_as<int>("Default1"));
    CHECK_THROWS(trans->get_as<int>("Default2"));
    CHECK("Chocobo" == trans->get_as<std::string>("Default2"));
  }

}// End TestCase :

TEST_CASE("kv_db_rocksdb_testcase listing") {

  fabko::kv_rocksdb::initializer_type arg {"tmp.db"};
  fabko::kv_rocksdb_instance rocks_db(arg);

  auto trans = rocks_db.make_transaction();

  trans->set({"Key1_chocobo1", "chocobo1"});
  trans->set({"Key1_chocobo2", "chocobo2"});
  trans->set({"Key1_chocobo3", "chocobo3"});
  trans->set({"Key1_chocobo4", "chocobo4"});
  trans->set({"Key1_chocobo5", "chocobo5"});
  trans->set({"Key2_chocobo1", "chocobo1"});
  trans->set({"Key2_chocobo2", "chocobo2"});
  trans->set({"Key3_chocobo1", "chocobo1"});

  SECTION("test listing : default") {
    std::vector<std::pair<std::string, std::string>> listing;

    trans->list("Key1", [&listing](const std::string& key, const std::string& value) {
      listing.emplace_back(key, value);
    });

    CHECK(5 == listing.size());
    CHECK("Key1_chocobo1" == listing[0].first);
    CHECK("chocobo1" == listing[0].second);

    CHECK("Key1_chocobo2" == listing[1].first);
    CHECK("chocobo2" == listing[1].second);

    CHECK("Key1_chocobo3" == listing[2].first);
    CHECK("chocobo3" == listing[2].second);

    CHECK("Key1_chocobo4" == listing[3].first);
    CHECK("chocobo4" == listing[3].second);

    CHECK("Key1_chocobo5" == listing[4].first);
    CHECK("chocobo5" == listing[4].second);
  }

  SECTION("test listing : defined end") {
    std::vector<std::pair<std::string, std::string>> listing;

    trans->list("Key1_chocobo3", "Key2_chocobo2", [&listing](const std::string& key, const std::string& value) {
      listing.emplace_back(key, value);
    });

    CHECK(4 == listing.size());
    CHECK("Key1_chocobo3" == listing[0].first);
    CHECK("chocobo3" == listing[0].second);

    CHECK("Key1_chocobo4" == listing[1].first);
    CHECK("chocobo4" == listing[1].second);

    CHECK("Key1_chocobo5" == listing[2].first);
    CHECK("chocobo5" == listing[2].second);

    CHECK("Key2_chocobo1" == listing[3].first);
    CHECK("chocobo1" == listing[3].second);
  }

  SECTION("test listing : defined end after whole key set") {
    std::vector<std::pair<std::string, std::string>> listing;

    trans->list("Key2_chocobo2", "Key4", [&listing](const std::string& key, const std::string& value) {
      listing.emplace_back(key, value);
    });

    CHECK(2 == listing.size());
    CHECK("Key2_chocobo2" == listing[0].first);
    CHECK("chocobo2" == listing[0].second);

    CHECK("Key3_chocobo1" == listing[1].first);
    CHECK("chocobo1" == listing[1].second);
  }

  SECTION("test listing : nothing in the range") {
    std::vector<std::pair<std::string, std::string>> listing;

    trans->list("Key42", [&listing](const std::string& key, const std::string& value) {
      listing.emplace_back(key, value);
    });

    CHECK(listing.empty());
  }

}

TEST_CASE("kv_db_rocksdb_testcase default setting", "[fil][kv_db][rocksdb]") {
  fabko::kv_rocksdb::initializer_type arg {"tmp.db", {{"1", "White Chocobo"}, {"4", "Black Chocobo"}, {"2", "Green Chocobo"}, {"3", "Yellow Chocobo"}}};
  fabko::kv_rocksdb_instance rocks_db(arg);

  auto trans = rocks_db.make_transaction();

  SECTION("Test default key/values are set") {
    auto res = trans->multi_get({"1", "2", "3", "4"});

    CHECK(4 == res.size());
    CHECK("1" == res.at(0).first);
    CHECK("White Chocobo" == res.at(0).second);

    CHECK("2" == res.at(1).first);
    CHECK("Green Chocobo" == res.at(1).second);

    CHECK("3" == res.at(2).first);
    CHECK("Yellow Chocobo" == res.at(2).second);

    CHECK("4" == res.at(3).first);
    CHECK("Black Chocobo" == res.at(3).second);
  }// End section : Test default key/values are set


}// End TestCase : kv_db_rocksdb_testcase