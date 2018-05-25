#include <fstream>
#include <iostream>

#include "fmt/format.h"

#include "ioutils.hpp"
#include "utilities.hpp"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

TEST_CASE("Path related functions", "basic") {
    std::string s0("/foo/foo");
    std::string s1("/foo/foo/////");
    std::string s2("/foo/foo");
    ioutils::remove_trailing_slash(s1);
    CHECK_THAT(s1, Equals(s0));
	
    ioutils::remove_trailing_slash(s2);
    CHECK_THAT(s2, Equals(s0));
}
