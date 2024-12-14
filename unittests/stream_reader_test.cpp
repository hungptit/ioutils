#include "fmt/base.h"
#include "ioutils/fdreader.hpp"
#include "ioutils/ioutils.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simple reader") {}

TEST_CASE("Basic tests for the StreamReader class") {
    using Policy = ioutils::StreamReaderAppendPolicy<1 << 16>;

    SECTION("Read data from a text file") {
        const std::string datafile("data.log");
        auto results = ioutils::read(datafile.c_str());
        fmt::print("{}\n", results);
        CHECK(results.find("xml.hpp") != std::string::npos);

        ioutils::StreamReader<Policy> reader;
        reader(datafile.c_str());
        CHECK(results == reader.data);
    }

    SECTION("Try to read from an inexisting file") {
        const std::string datafile("bad_input_data_file");
        ioutils::StreamReader<Policy> reader;
        reader(datafile.c_str());
        REQUIRE(reader.data.empty());
    }
}
