#include "ioutils/fdwriter.hpp"
#include "ioutils/utilities.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simplify paths") {
    SECTION("Case 1") { CHECK(ioutils::path::simplify_path("") == ""); }
    SECTION("Case 2") { CHECK(ioutils::path::simplify_path("/") == "/"); }
    SECTION("Case 3") { CHECK(ioutils::path::simplify_path("////////.") == "/"); }
    SECTION("Case 4") { CHECK(ioutils::path::simplify_path("////////") == "/"); }
    SECTION("Case 5") { CHECK(ioutils::path::simplify_path("//home/.//.///.") == "/home"); }
    SECTION("Case 6") { CHECK(ioutils::path::simplify_path("//home/.//.///..") == "/"); }
    SECTION("Case 7") { CHECK(ioutils::path::simplify_path("//..") == "/"); }
    SECTION("Case 8") { CHECK(ioutils::path::simplify_path("//../home") == "/home"); }
    SECTION("Case 9") { CHECK(ioutils::path::simplify_path("//home/boo/../goo//..//.//.///..") == "/"); }
    SECTION("Case 10") {
        CHECK(ioutils::path::simplify_path("/..//home/boo/../goo////.//.///..") == "/home");
    }
    SECTION("Case 11") {
        CHECK(ioutils::path::simplify_path("/..//home/../boo/../goo//..//.//.///..") == "/");
    }
    SECTION("Case 11") { CHECK(ioutils::path::simplify_path("/a//b////c/d//././/..") == "/a/b/c"); }
    SECTION("Case 12") { CHECK(ioutils::path::simplify_path("/..//a//b////c/d//././/..") == "/a/b/c"); }
    SECTION("Case 12") { CHECK(ioutils::path::simplify_path("../src////") == "../src"); }
    SECTION("Case 12") { CHECK(ioutils::path::simplify_path("../../../foo/../src////") == "../../../src"); }
    SECTION("Case 13") { CHECK(ioutils::path::simplify_path(".") == "."); }
    SECTION("Case 14") { CHECK(ioutils::path::simplify_path("..") == ".."); }
    SECTION("Case 15") { CHECK(ioutils::path::simplify_path("../") == ".."); }
    SECTION("Case 16") { CHECK(ioutils::path::simplify_path("././") == "."); }
    SECTION("Case 16") { CHECK(ioutils::path::simplify_path("////////") == "/"); }
}

TEST_CASE("Console") {
    ioutils::StreamWriter console(ioutils::StreamWriter::STDOUT);
    std::string s1("Hello");
    std::string s2(" world!\n");
    console.write(s1.data(), s1.size());
    console.write(s2.data(), s2.size());
}
