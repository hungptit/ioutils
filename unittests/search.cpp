#include <fstream>
#include <iostream>
#include <string>

#include "fmt/format.h"
#include "search.hpp"
#include "search_policies.hpp"
#include "regex_policies.hpp"
#include "store_policy.hpp"
#include "temporary_dir.hpp"
#include "test_data.hpp"
#include "filesystem.hpp"

#include "fmt/format.h"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

using namespace std;
// using namespace boost;

TEST_CASE("Valid dir", "basic") {
    CHECK(!ioutils::filesystem::is_valid_dir(".git"));
    CHECK(!ioutils::filesystem::is_valid_dir("."));
    CHECK(!ioutils::filesystem::is_valid_dir(".."));
    CHECK(ioutils::filesystem::is_valid_dir("foo"));
    CHECK(ioutils::filesystem::is_valid_dir(".foo"));
    CHECK(ioutils::filesystem::is_valid_dir(".gitboo"));
}

TEST_CASE("Utility function", "basic") {
    TestData test(false);
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

    SECTION("Search for files using DFS algorithm and print results") {
        ioutils::FileSearch<ioutils::ConsolePolicy> search;
        fmt::print("DFS - ConsolePolicy:\n");
        search.dfs(std::vector<std::string>{p});
    }

    SECTION("Search for files in a given folder using DFS algorithm and store results") {
        ioutils::FileSearch<ioutils::StorePolicy> search;
        fmt::print("DFS - StorePolicy:\n");
        search.dfs(std::vector<std::string> {p});
        CHECK(search.get_files().size() == 12);
        for (auto item : search.get_files()) {
            fmt::print("{}\n", item);
        }
    }

    SECTION("Search for files in a given folder and return a list of files") {
        ioutils::FileSearch<ioutils::ConsolePolicy> search;
        fmt::print("BFS - ConsolePolicy:\n");
        search.bfs(std::vector<std::string> {p});
    }

    SECTION("Search for files in a given folder using BFS algorithm and store results") {
        ioutils::FileSearch<ioutils::StorePolicy> search;
        fmt::print("BFS - StorePolicy:\n");
        search.bfs(std::vector<std::string> {p});
        CHECK(search.get_files().size() == 12);
    }
}

TEST_CASE("Search with regex", "basic") {
    TestData test(false);
    filesystem::path tmpdir = test.get_path();
    std::string p = tmpdir.string();

    fmt::print("RegexStorePolicy - StorePolicy:\n");
    using Matcher = utils::hyperscan::RegexMatcher;
    using Policy = ioutils::RegexStorePolicy<Matcher>;
    using Search = typename ioutils::FileSearch<Policy>;
    int mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
    Search search("cpp$", mode);
    search.dfs(std::vector<std::string>{p});
    for (auto afile : search.get_files()) {
        fmt::print("{}\n", afile);
    }
    CHECK(search.get_files().size() == 3);
}
