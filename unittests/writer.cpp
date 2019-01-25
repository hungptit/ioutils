#include "fmt/format.h"
#include "fdwriter.hpp"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

TEST_CASE("Writer class") {
    std::string s1 = "Hello";
    std::string s2 = " world!";
    std::string eol = "\n";
    SECTION("Default output") {
        ioutils::Writer writer;
        writer.write(s1.data(), s1.size());
        writer.write(s2.data(), s2.size());
        writer.write(eol.data(), eol.size());
    }
    
    SECTION("Provide file descriptor") {
        ioutils::Writer writer(STDERR_FILENO);
        writer.write(s1.data(), s1.size());
        writer.write(s2.data(), s2.size());
        writer.write(eol.data(), eol.size());
    }

}

TEST_CASE("Test color") {
    fmt::print("\\e[41;4;33m[{}\n", "My string");
}
