#include <fstream>
#include <iostream>
#include <string>

#include "fmt/format.h"
#include "search.hpp"
#include "temporary_dir.hpp"
#include "test_data.hpp"

#include "fmt/format.h"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

using namespace boost;

TEST_CASE("Utility function", "basic") {
    TestData test(true);
    filesystem::path tmpdir = test.get_path();
    std::string p = tmpdir.string();
    struct stat props;

    SECTION("Path related functions") {
        stat(p.data(), &props);
        CHECK(filesystem::exists(p));
        CHECK(ioutils::filesystem::exists(p.data()));
        CHECK(ioutils::filesystem::is_directory(props.st_mode));
        CHECK(!ioutils::filesystem::is_regular_file(props.st_mode));
        CHECK(!ioutils::filesystem::is_symlink(props.st_mode));

        std::string afile = p + "/src/test.cpp";
        CHECK(ioutils::filesystem::exists(afile.data()));
        stat(afile.data(), &props);
        CHECK(!ioutils::filesystem::is_directory(props.st_mode));
        CHECK(ioutils::filesystem::is_regular_file(props.st_mode));
        CHECK(!ioutils::filesystem::is_symlink(props.st_mode));
    }

    SECTION("Search for files in a given folder") {
        ioutils::FileSearch search;
        search.dfs({p});
    }

    SECTION("Search for files in a given folder and return a list of files") {
        ioutils::FileSearch search;
        search.dfs({p});
    }
}
