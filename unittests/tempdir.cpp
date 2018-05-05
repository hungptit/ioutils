#include <fstream>
#include <iostream>

#include "fmt/format.h"
#include "temporary_dir.hpp"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

using namespace boost;
TEST_CASE("Default constructor", "basic") {
    filesystem::path p;

    {
        ioutils::TemporaryDirectory tmpdir;
        fmt::print("{}\n", tmpdir.get_path().string());
        p = tmpdir.get_path();

        // The temporary folder must exist.
        CHECK(filesystem::exists(tmpdir.get_path()));
    }

    // The temporary folder must be destroyed already.
    CHECK(!filesystem::exists(p));
    fmt::print("{}\n", p.string());

    // The folder's name is randomized.
    ioutils::TemporaryDirectory tmpdir;
    CHECK(p.string() != tmpdir.get_path().string());
}

TEST_CASE("With given parent folder", "basic") {
    filesystem::path p;

    {
        ioutils::TemporaryDirectory tmpdir("./");
        fmt::print("{}\n", tmpdir.get_path().string());
        p = tmpdir.get_path();

        // The temporary folder must exist.
        CHECK(filesystem::exists(tmpdir.get_path()));
    }

    // The temporary folder must be destroyed already.
    CHECK(!filesystem::exists(p));
    fmt::print("{}\n", p.string());

    // The folder's name is randomized.
    ioutils::TemporaryDirectory tmpdir;
    CHECK(p.string() != tmpdir.get_path().string());
}
