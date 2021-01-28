#include <fstream>
#include <iostream>

#include "fmt/format.h"
#include "ioutils.hpp"
#include "read_policies.hpp"
#include "reader.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

TEST_CASE("Simple reader") {
    const std::string datafile("data.log");
    auto results = ioutils::read(datafile.c_str());
    fmt::print("{}\n", results);
    CHECK(results.find("xml.hpp") != std::string::npos);

    using Policy = ioutils::AppendPolicy<std::string>;
    ioutils::FileReader<Policy, 1 << 16> reader;
    reader(datafile.c_str());
    CHECK(results == reader.get_data());
}
