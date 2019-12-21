#include <fstream>
#include <iostream>

#include "fmt/format.h"
#include "temporary_dir.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

namespace fs = std::filesystem;
TEST_CASE("Default constructor") {
    fs::path p;

    {
        ioutils::TemporaryDirectory tmpdir;
        fmt::print("{}\n", tmpdir.get_path().string());
        const auto p = tmpdir.get_path();

        // The temporary folder must exist.
        CHECK(fs::exists(tmpdir.get_path()));
    }

    // The temporary folder must be destroyed already.
    CHECK(!fs::exists(p));
    fmt::print("{}\n", p.string());

    // The folder's name is randomized.
    ioutils::TemporaryDirectory tmpdir;
    CHECK(p.string() != tmpdir.get_path().string());
}

TEST_CASE("With given parent folder") {
    fs::path p;

    {
        ioutils::TemporaryDirectory tmpdir("./");
        fmt::print("{}\n", tmpdir.get_path().string());
        p = tmpdir.get_path();

        // The temporary folder must exist.
        CHECK(fs::exists(tmpdir.get_path()));
    }

    // The temporary folder must be destroyed already.
    CHECK(!fs::exists(p));
    fmt::print("{}\n", p.string());

    // The folder's name is randomized.
    ioutils::TemporaryDirectory tmpdir;
    CHECK(p.string() != tmpdir.get_path().string());
}
