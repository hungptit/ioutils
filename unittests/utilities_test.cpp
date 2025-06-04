#include "ioutils/fdwriter.hpp"
#include "ioutils/utilities.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simplify paths") {
    SECTION("Empty and root paths") {
        CHECK(ioutils::path::simplify_path("") == "");
        CHECK(ioutils::path::simplify_path("/") == "/");
        CHECK(ioutils::path::simplify_path("////////") == "/");
        CHECK(ioutils::path::simplify_path("////////.") == "/");
    }

    SECTION("Current directory") {
        CHECK(ioutils::path::simplify_path(".") == ".");
        CHECK(ioutils::path::simplify_path("./") == ".");
        CHECK(ioutils::path::simplify_path("./.") == ".");
        CHECK(ioutils::path::simplify_path("././") == ".");
        CHECK(ioutils::path::simplify_path(".//.") == ".");
        CHECK(ioutils::path::simplify_path(".///.") == ".");
    }

    SECTION("Parent directory") {
        CHECK(ioutils::path::simplify_path("..") == "..");
        CHECK(ioutils::path::simplify_path("../") == "..");
        CHECK(ioutils::path::simplify_path("../../") == "../..");
        CHECK(ioutils::path::simplify_path("../../../") == "../../..");
    }

    SECTION("Simple absolute paths") {
        CHECK(ioutils::path::simplify_path("/home") == "/home");
        CHECK(ioutils::path::simplify_path("/home/") == "/home");
        CHECK(ioutils::path::simplify_path("/home/user") == "/home/user");
        CHECK(ioutils::path::simplify_path("/home/user/") == "/home/user");
    }

    SECTION("Simple relative paths") {
        CHECK(ioutils::path::simplify_path("home") == "home");
        CHECK(ioutils::path::simplify_path("home/") == "home");
        CHECK(ioutils::path::simplify_path("home/user") == "home/user");
        CHECK(ioutils::path::simplify_path("home/user/") == "home/user");
    }

    SECTION("Paths with current directory references") {
        CHECK(ioutils::path::simplify_path("/home/./user") == "/home/user");
        CHECK(ioutils::path::simplify_path("/home/././user") == "/home/user");
        CHECK(ioutils::path::simplify_path("/home/./user/.") == "/home/user");
        CHECK(ioutils::path::simplify_path("/home/./user/./") == "/home/user");
        CHECK(ioutils::path::simplify_path("./home/./user") == "home/user");
    }

    SECTION("Paths with parent directory references") {
        CHECK(ioutils::path::simplify_path("/home/../user") == "/user");
        CHECK(ioutils::path::simplify_path("/home/user/../doc") == "/home/doc");
        CHECK(ioutils::path::simplify_path("/home/../user/../doc") == "/doc");
        CHECK(ioutils::path::simplify_path("../home/../user") == "../user");
        CHECK(ioutils::path::simplify_path("home/../user") == "user");
    }

    SECTION("Complex path combinations") {
        CHECK(ioutils::path::simplify_path("/home/./user/../doc/./file") == "/home/doc/file");
        CHECK(ioutils::path::simplify_path("/home/../user/./doc/../file") == "/user/file");
        CHECK(ioutils::path::simplify_path("home/./user/../doc/./file") == "home/doc/file");
        CHECK(ioutils::path::simplify_path("home/../user/./doc/../file") == "user/file");
    }

    SECTION("Edge cases with multiple slashes") {
        CHECK(ioutils::path::simplify_path("//home//user//") == "/home/user");
        CHECK(ioutils::path::simplify_path("home//user//") == "home/user");
        CHECK(ioutils::path::simplify_path("//home//user//doc//") == "/home/user/doc");
        CHECK(ioutils::path::simplify_path("home//user//doc//") == "home/user/doc");
    }

    SECTION("Complex parent directory navigation") {
        CHECK(ioutils::path::simplify_path("/home/boo/../goo/..") == "/home");
        CHECK(ioutils::path::simplify_path("/home/boo/../goo/../doc") == "/home/doc");
        CHECK(ioutils::path::simplify_path("home/boo/../goo/..") == "home");
        CHECK(ioutils::path::simplify_path("home/boo/../goo/../doc") == "home/doc");
    }

    SECTION("Root directory edge cases") {
        CHECK(ioutils::path::simplify_path("/..") == "/");
        CHECK(ioutils::path::simplify_path("/../") == "/");
        CHECK(ioutils::path::simplify_path("/../..") == "/");
        CHECK(ioutils::path::simplify_path("/../../") == "/");
    }

    SECTION("Mixed relative and absolute paths") {
        CHECK(ioutils::path::simplify_path("/home/../user/./doc/../../file") == "/file");
        CHECK(ioutils::path::simplify_path("home/../user/./doc/../../file") == "file");
        CHECK(ioutils::path::simplify_path("/home/../user/./doc/../../../file") == "/file");
        CHECK(ioutils::path::simplify_path("home/../user/./doc/../../../file") == "../file");
    }

    SECTION("Special cases") {
        CHECK(ioutils::path::simplify_path("...") == "...");
        CHECK(ioutils::path::simplify_path("....") == "....");
        CHECK(ioutils::path::simplify_path("/...") == "/...");
        CHECK(ioutils::path::simplify_path("/....") == "/....");
    }
}

TEST_CASE("Console") {
    ioutils::StreamWriter console(ioutils::StreamWriter::STDOUT);
    std::string s1("Hello");
    std::string s2(" world!\n");
    console.write(s1.data(), s1.size());
    console.write(s2.data(), s2.size());
}
