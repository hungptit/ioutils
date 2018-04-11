#include <fstream>
#include <iostream>

#include "fmt/format.h"

#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/xml.hpp"

#include <boost/iostreams/device/mapped_file.hpp>

#include "boost_memmap.hpp"
#include "ioutils.hpp"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

TEST_CASE("Simple reader", "basic") {
	const std::string datafile("data.log");
	auto results = ioutils::read(datafile.c_str());
	fmt::print("{}\n", results);
	CHECK(results.find("xml.hpp") != std::string::npos);

	ioutils::FileReader<1<<16, ioutils::AppendPolicy> reader;
	reader(datafile.c_str());
	CHECK_THAT(results, Equals(reader.policy.data));
}
