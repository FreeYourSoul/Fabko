#include <algorithm>
#include <print>
#include <ranges>
#include <string>

#include "soa.h"

int main() {

    fil::soa<int, double> table;

    const auto id1 = table.insert(42, 13.37);
    const auto id2 = table.insert(43, 13.47);
    const auto id3 = table.insert(44, 13.57);
    const auto id4 = table.insert(45, 13.67);

    auto data1 = table[id1];
    auto data2 = table[id2];
    auto data3 = table[id3];

    std::println("integer : {} :: double : {} ", get<0>(data1), get<1>(data1));
    std::println("integer : {} :: double : {} ", get<0>(data2), get<1>(data2));
    std::println("integer : {} :: double : {} ", get<0>(data3), get<1>(data3));

    std::println("-----");
    for (auto it = table.begin(); it != table.end(); ++it) {
        std::println("iterator loop -- integer : {} :: double : {} ", get<0>(*it), get<1>(*it));
    }

    std::println("-----");
    for (auto s : table) {
        std::println("forloop -- integer : {} :: double : {} ", get<0>(s), get<1>(s));
    }

    std::println("-----");
    for (auto [i, d] : table) {
        std::println("deconstruction -- integer : {} :: double {}", i, d);
    }

    std::println("-----");
    const auto v = std::ranges::fold_left(table, 0z, [](std::size_t res, const auto& v) {
        const auto& [i, d] = v;
        return res + i;
    });
    std::println("accumulate :: {}", v);

    std::println("-----");
    const auto it = std::ranges::find_if(table, fil::soa_apply([](int i, double d) { //
                                             return i == 44;
                                         }));

    std::println("find if found :: {} ", it != table.end());

    //
    // deletion quick test

    std::println("-----");
    std::println("before deletion    :: size {}", table.size());
    std::println("before deletion    :: id1({}) id2({}) id3({}) id4({})",
                 table.has_id(id1),
                 table.has_id(id2),
                 table.has_id(id3),
                 table.has_id(id4));

    for (auto s : table) {
        std::println("forloop -- integer : {} :: double : {} ", get<0>(s), get<1>(s));
    }
    bool res = table.erase(id3);

    std::println("delete one element :: success : {} : size {}", res, table.size());
    std::println("after deletion     :: id1({}) id2({}) id3({}) id4({})",
                 table.has_id(id1),
                 table.has_id(id2),
                 table.has_id(id3),
                 table.has_id(id4));
    std::println("after deletion     :: id3 exists : {}", table.has_id(id3));

    auto id3_bis = table.insert(50, 50.50);
    std::println("after insertion");

    std::println("-----");
    std::println("add id3 again     :: id3 != id3_bis == {} :: generation working if true", id3 != id3_bis);

    std::println("id3_bis -- integer: {} :: double: {}", get<0>(table[id3_bis]), get<1>(table[id3_bis]));

    std::println("--size {}", table.size());
    std::ranges::for_each(table, fil::select<std::size_t {1}>([](double d) { std::println("select :: double : {}", d); }));

    return 0;
}
