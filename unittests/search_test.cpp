#include "ioutils/filesystem.hpp"
#include "ioutils/find.hpp"
#include "ioutils/search.hpp"

#include "fmt/format.h"

#include "ioutils/locate.hpp"
#include "ioutils/regex_store_policies.hpp"
#include "ioutils/search_params.hpp"
#include "ioutils/search_policies.hpp"
#include "ioutils/simple_store_policy.hpp"
#include "ioutils/temporary_dir.hpp"

#include "test_data.hpp"

#include <string>

#include "fmt/format.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

using namespace std;

TEST_CASE("Valid dir") {
    CHECK(!ioutils::filesystem::is_valid_dir("."));
    CHECK(!ioutils::filesystem::is_valid_dir(".."));
    CHECK(ioutils::filesystem::is_valid_dir(".git"));
    CHECK(ioutils::filesystem::is_valid_dir("foo"));
    CHECK(ioutils::filesystem::is_valid_dir(".foo"));
    CHECK(ioutils::filesystem::is_valid_dir(".gitboo"));
}

TEST_CASE("Utility function") {
    TestData test(false);
    filesystem::path tmpdir = test.get_path();
    std::string p = tmpdir.string();
    struct stat props;

    SUBCASE("Path related functions") {
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

    SUBCASE("Search for files using DFS algorithm and print results") {
        // TODO: Update this.
        // ioutils::filesystem::DefaultSearch<ioutils::find::SimplePolicy> search;
        // fmt::print("DFS - ConsolePolicy:\n");
        // search.traverse(p);
    }

    SUBCASE("Search for files in a given folder using DFS algorithm and store results") {
        SUBCASE("Ignore dir") {
            ioutils::search::InputParams params;
            params.flags |= ioutils::search::IGNORE_DIR;
            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().size() == 14);
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }

        SUBCASE("Ignore files") {
            ioutils::search::InputParams params;
            params.flags |= ioutils::search::IGNORE_DIR;
            params.flags |= ioutils::search::IGNORE_FILE;
            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().size() == 1); // Should not see anything.
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }

        SUBCASE("Ignore files") {
            ioutils::search::InputParams params;
            params.flags |= ioutils::search::IGNORE_DIR;
            params.flags |= ioutils::search::IGNORE_FILE;
            params.flags |= ioutils::search::IGNORE_SYMLINK;
            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().size() == 0); // Should not see anything.
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }

        SUBCASE("Include all") {
            ioutils::search::InputParams params;
            params.flags = ioutils::search::DFS;
            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            fmt::print("DFS - StorePolicy:\n");
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().size() == 21);
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }

        SUBCASE("Include all with the given level") {
            ioutils::search::InputParams params;
            params.maxdepth = 0;
            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            fmt::print("DFS - StorePolicy:\n");
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().size() == 8);
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }
    }

    SUBCASE("Search for files in a given folder and return a list of files") {
        ioutils::search::InputParams params;
        ioutils::filesystem::DefaultSearch<ioutils::find::SimplePolicy> search(params);
        fmt::print("DFS - SimplePolicy:\n");
        search.bfs(p);
    }

    SUBCASE("Search for files in a given folder using BFS algorithm and store results") {
        ioutils::search::InputParams params;
        ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
        fmt::print("BFS - StorePolicy:\n");
        search.bfs(p);
        CHECK(search.get_paths().size() == 21);
        for (auto p : search.get_paths()) {
            fmt::print("{}\n", p);
        }
    }
}

TEST_CASE("Search with regex") {
    TestData test(false);
    filesystem::path tmpdir = test.get_path();
    std::string p = tmpdir.string();

    fmt::print("RegexStorePolicy - StorePolicy:\n");
    using Matcher = utils::hyperscan::RegexMatcher;
    using Policy = ioutils::RegexStorePolicy<Matcher>;
    using Search = typename ioutils::filesystem::DefaultSearch<Policy>;
    ioutils::search::InputParams params;
    params.flags |= ioutils::search::IGNORE_SYMLINK;
    params.path_regex = "[.]cpp";
    Search search(params);
    search.traverse(std::vector<std::string>{p});
    for (auto p : search.get_paths()) {
        fmt::print("{}\n", p);
    }
    CHECK(search.get_paths().size() == 4);
}
