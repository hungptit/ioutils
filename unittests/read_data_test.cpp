#include "fmt/base.h"
#include "ioutils/ioutils.hpp"
#include "ioutils/read_policies.hpp"
#include "ioutils/reader.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simple reader") {
    using Policy = ioutils::AppendPolicy<std::string>;

    const std::string datafile("data.log");
    auto results = ioutils::read(datafile.c_str());
    fmt::print("{}\n", results);
    CHECK(results.find("xml.hpp") != std::string::npos);
    ioutils::FileReader<Policy, 1 << 16> reader;
    reader(datafile.c_str());
    CHECK(results == reader.get_data());
}

TEST_CASE("Basic tests for the StreamReader class") {
    SECTION("Try to read from an inexisting file") {
        using Policy = ioutils::AppendPolicy<std::string>;
        const std::string datafile("bad_input_data_file");
        ioutils::FileReader<Policy, 1 << 16> reader;
        reader(datafile.c_str());
        REQUIRE(reader.get_data().empty());
    }
}
