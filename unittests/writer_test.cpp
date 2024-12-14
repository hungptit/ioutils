#include "ioutils/console.hpp"
#include "ioutils/fdwriter.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Writer class") {
    std::string s1 = "Hello";
    std::string s2 = " world!";
    std::string eol = "\n";

    SECTION("Default output") {
        ioutils::StreamWriter writer(ioutils::StreamWriter::STDOUT);
        writer.write(s1.data(), s1.size());
        writer.write(s2.data(), s2.size());
        writer.write(eol.data(), eol.size());
    }

    SECTION("Provide file descriptor") {
        ioutils::StreamWriter writer(ioutils::StreamWriter::STDERR, 4);
        writer.write(s1.data(), s1.size());
        writer.write(s2.data(), s2.size());
        writer.write(eol.data(), eol.size());
    }
}

TEST_CASE("Test color") {
    ioutils::StreamWriter writer(ioutils::StreamWriter::STDERR, 4);
    writer.write(ioutils::console::FOLDER_COLOR.data(), ioutils::console::FOLDER_COLOR.size());
    writer.put('H');
    writer.write(ioutils::console::FILE_COLOR.data(), ioutils::console::FILE_COLOR.size());
    writer.put('H');
    writer.write(ioutils::console::SYMLINK_COLOR.data(), ioutils::console::SYMLINK_COLOR.size());
    writer.put('H');
    writer.write(ioutils::console::RESET_COLOR.data(), ioutils::console::RESET_COLOR.size());
    writer.put('H');
    writer.eol();
}
