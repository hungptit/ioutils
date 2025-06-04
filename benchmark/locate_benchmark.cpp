#include "catch2/catch_test_macros.hpp"
#include <cstdlib>
#include <string>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

namespace {
    constexpr int number_of_samples = 10;
    constexpr int number_of_operations = 5;

    auto test(const std::string &command, const std::string &regex) -> int {
        std::string buffer = command + regex + " > /tmp/output.log";
        return system(buffer.data());
    }

    auto test_gnu_locate_regex(const std::string &command, const std::string &regex) -> int {
        const std::string dbfile = "locate.db";
        std::string buffer = command + " -d " + dbfile + " --regex " + regex + " > /tmp/locate.log";
        return system(buffer.data());
    }

    auto test_locate_regex(const std::string &command, const std::string &regex) -> int {
        const std::string dbfile = ".database";
        std::string buffer = command + " -d " + dbfile + " " + regex + " > /tmp/fast-locate.log";
        return system(buffer.data());
    }
    constexpr char pattern1[] = "zstd/.*doc/README[.]md$";
} // namespace

TEST_CASE("Basic benchmark") {

    auto bm = ankerl::nanobench::Bench().warmup(3).minEpochIterations(10);
    bm.run("locate - mid", []() { test(std::string("locate") + " -d locate_db --regex ", pattern1); });
    bm.run("locate - mid",
           []() { test(std::string("../commands/fast-locate") + " -d .database", pattern1); });
}
