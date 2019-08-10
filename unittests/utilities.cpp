#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include <fstream>
#include <iostream>
#include "fmt/format.h"

#include "ioutils.hpp"
#include "utilities.hpp"
#include "fdwriter.hpp"

TEST_CASE("Simplify paths") {
    SUBCASE("Case 1") { CHECK(ioutils::path::simplify_path("") == ""); }
    SUBCASE("Case 2") { CHECK(ioutils::path::simplify_path("/") == "/"); }
    SUBCASE("Case 3") { CHECK(ioutils::path::simplify_path("////////.") == "/"); }
    SUBCASE("Case 4") { CHECK(ioutils::path::simplify_path("////////") == "/"); }
    SUBCASE("Case 5") { CHECK(ioutils::path::simplify_path("//home/.//.///.") == "/home"); }
    SUBCASE("Case 6") { CHECK(ioutils::path::simplify_path("//home/.//.///..") == "/"); }
    SUBCASE("Case 7") { CHECK(ioutils::path::simplify_path("//..") == "/"); }
    SUBCASE("Case 8") { CHECK(ioutils::path::simplify_path("//../home") == "/home"); }
    SUBCASE("Case 9") { CHECK(ioutils::path::simplify_path("//home/boo/../goo//..//.//.///..") == "/"); }
    SUBCASE("Case 10") { CHECK(ioutils::path::simplify_path("/..//home/boo/../goo////.//.///..") == "/home"); }
    SUBCASE("Case 11") { CHECK(ioutils::path::simplify_path("/..//home/../boo/../goo//..//.//.///..") == "/"); }
    SUBCASE("Case 11") { CHECK(ioutils::path::simplify_path("/a//b////c/d//././/..") == "/a/b/c"); }
    SUBCASE("Case 12") { CHECK(ioutils::path::simplify_path("/..//a//b////c/d//././/..") == "/a/b/c"); }
    SUBCASE("Case 12") { CHECK(ioutils::path::simplify_path("../src////") == "../src"); }
    SUBCASE("Case 12") { CHECK(ioutils::path::simplify_path("../../../foo/../src////") == "../../../src"); }
    SUBCASE("Case 13") { CHECK(ioutils::path::simplify_path(".") == "."); }
    SUBCASE("Case 14") { CHECK(ioutils::path::simplify_path("..") == ".."); }
    SUBCASE("Case 15") { CHECK(ioutils::path::simplify_path("../") == ".."); }
    SUBCASE("Case 16") { CHECK(ioutils::path::simplify_path("././") == "."); }
    SUBCASE("Case 16") { CHECK(ioutils::path::simplify_path("////////") == "/"); }
}

TEST_CASE("Console") {
    ioutils::StreamWriter console(ioutils::StreamWriter::STDOUT);
    std::string s1("Hello");
    std::string s2(" world!\n");
    console.write(s1.data(), s1.size());
    console.write(s2.data(), s2.size());
}
