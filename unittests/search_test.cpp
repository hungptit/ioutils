#include "fmt/base.h"
#include "ioutils/filesystem.hpp"
#include "ioutils/find.hpp"
#include "ioutils/regex_store_policies.hpp"
#include "ioutils/search.hpp"
#include "ioutils/simple_store_policy.hpp"
#include "test_data.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

using ioutils::TestData;

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
    const auto tmpdir = test.get_path();
    std::string p = tmpdir.string();
    struct stat props;

    SECTION("Path related functions") {
        stat(p.data(), &props);
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
        // TODO: Update this.
        // ioutils::filesystem::DefaultSearch<ioutils::find::SimplePolicy> search;
        // fmt::print("DFS - ConsolePolicy:\n");
        // search.traverse(p);
    }

    SECTION("Search for files in a given folder using DFS algorithm and store results") {
        SECTION("Ignore dir") {
            ioutils::SearchInputArguments params;
            params.flags |= ioutils::IGNORE_DIR;
            params.print();

            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().size() == 14);
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }

        SECTION("Ignore files") {
            ioutils::SearchInputArguments params;
            params.flags |= ioutils::IGNORE_DIR;
            params.flags |= ioutils::IGNORE_FILE;
            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().size() == 1); // Should not see anything.
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }

        SECTION("Ignore files") {
            ioutils::SearchInputArguments params;
            params.flags |= ioutils::IGNORE_DIR;
            params.flags |= ioutils::IGNORE_FILE;
            params.flags |= ioutils::IGNORE_SYMLINK;
            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().empty()); // Should not see anything.
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }

        SECTION("Include all") {
            ioutils::SearchInputArguments params;
            params.flags = ioutils::DFS;
            ioutils::filesystem::DefaultSearch<ioutils::StorePolicy> search(params);
            fmt::print("DFS - StorePolicy:\n");
            search.traverse(std::vector<std::string>{p});
            CHECK(search.get_paths().size() == 21);
            for (auto item : search.get_paths()) {
                fmt::print("{}\n", item);
            }
        }

        SECTION("Include all with the given level") {
            ioutils::SearchInputArguments params;
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

    SECTION("Search for files in a given folder and return a list of files") {
        ioutils::SearchInputArguments params;
        ioutils::filesystem::DefaultSearch<ioutils::find::SimplePolicy> search(params);
        fmt::print("DFS - SimplePolicy:\n");
        search.bfs(p);
    }

    SECTION("Search for files in a given folder using BFS algorithm and store results") {
        ioutils::SearchInputArguments params;
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
    const auto tmpdir = test.get_path();
    std::string p = tmpdir.string();
    fmt::print("RegexStorePolicy - StorePolicy:\n");
    using Matcher = utils::hyperscan::RegexMatcher;
    using Policy = ioutils::RegexStorePolicy<Matcher>;
    using Search = typename ioutils::filesystem::DefaultSearch<Policy>;
    ioutils::SearchInputArguments params;
    params.flags |= ioutils::IGNORE_SYMLINK;
    params.path_regex = "[.]cpp";
    Search search(params);
    search.traverse(std::vector<std::string>{p});
    for (auto p : search.get_paths()) {
        fmt::print("{}\n", p);
    }
    CHECK(search.get_paths().size() == 4);
}
