#include "fdwriter.hpp"
#include "fmt/format.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

TEST_CASE("Writer class") {
    std::string s1 = "Hello";
    std::string s2 = " world!";
    std::string eol = "\n";
    SUBCASE("Default output") {
        ioutils::StreamWriter writer(ioutils::StreamWriter::STDOUT);
        writer.write(s1.data(), s1.size());
        writer.write(s2.data(), s2.size());
        writer.write(eol.data(), eol.size());
    }

    SUBCASE("Provide file descriptor") {
        ioutils::StreamWriter writer(ioutils::StreamWriter::STDERR);
        writer.write(s1.data(), s1.size());
        writer.write(s2.data(), s2.size());
        writer.write(eol.data(), eol.size());
    }
}

TEST_CASE("Test color") { fmt::print("\\e[41;4;33m[{}\n", "My string"); }
