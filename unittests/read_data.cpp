#include <fstream>
#include <iostream>

#include "fmt/format.h"
#include "ioutils.hpp"
#include "reader.hpp"
#include "read_policies.hpp"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

TEST_CASE("Simple reader", "basic") {
    const std::string datafile("data.log");
    auto results = ioutils::read(datafile.c_str());
    fmt::print("{}\n", results);
    CHECK(results.find("xml.hpp") != std::string::npos);

    using Policy = ioutils::AppendPolicy<std::string>;
    ioutils::FileReader<Policy, 1 << 16> reader;
    reader(datafile.c_str());
    CHECK_THAT(results, Equals(reader.get_data()));
}
