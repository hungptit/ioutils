#include <catch2/catch_test_macros.hpp>
#include <reflex/input.h>

TEST_CASE("Basic examples") {
    SECTION("Find a matched line") {
        std::string line = "This is a sample line";
        std::string pattern = "a.*sample";
        reflex::Input input(line.data(), line.size());
        CHECK(input.size() == line.size());
    }
}
