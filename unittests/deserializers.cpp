#include <fstream>
#include <iostream>

#include "deserializers.hpp"
#include "fmt/format.h"
#include <cereal/archives/json.hpp>

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

namespace {

    struct Address {
        int number;
        std::string street;
        std::string city;
        std::string state;

        Address() : number(-1), street(), city(), state() {}
        Address(int num, const std::string &str, const std::string &ct, const std::string &st)
            : number(num), street(str), city(ct), state(st) {}

        template <class Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(number), CEREAL_NVP(street), CEREAL_NVP(city), CEREAL_NVP(state));
        }

        bool operator==(const Address &rhs) {
            return std::tie(number, street, city, state) ==
                   std::tie(rhs.number, rhs.street, rhs.city, rhs.state);
        }
    };
} // namespace

TEST_CASE("Basic tests") {
    Address addr(1, "Foo", "Boo", "Goo");
    ioutils::cereal::print<cereal::JSONOutputArchive>(addr, "Original data");

    std::string data_file = "data.json";

    // Write results to a file
    {
        std::ofstream os(data_file);
        cereal::JSONOutputArchive archive(os);
        archive(addr);
    }

    // Deserialize data from a file
    using Policy = typename ioutils::cereal::ReadPolicy<cereal::JSONInputArchive, Address>;
    ioutils::MMapReader<Policy> reader;
    reader(data_file.data());
    ioutils::cereal::print<cereal::JSONOutputArchive>(reader.results, "Read data");

    bool isok = reader.results == addr;
    CHECK(isok);
}
