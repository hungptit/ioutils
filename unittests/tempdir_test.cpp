#include "fmt/base.h"
#include "ioutils/temporary_dir.hpp"
#include <catch2/catch_test_macros.hpp>

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
