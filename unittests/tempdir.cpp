#include <fstream>
#include <iostream>

#include "fmt/format.h"
#include "temporary_dir.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

using namespace boost;
TEST_CASE("Default constructor") {
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

TEST_CASE("With given parent folder") {
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
