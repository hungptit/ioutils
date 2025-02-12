#include "catch2/catch_test_macros.hpp"
#include <cstdlib>
#include <string>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

namespace {
    constexpr int number_of_samples = 10;
    constexpr int number_of_operations = 1;
    const std::string find_cmd = "find";

    const std::string fast_find("../commands/fast-find ");
    const std::string tmp_file = "/tmp/output.log";

    // We must set the TEST_DIR environment variable when running the benchmark.
    const auto test_path = std::string(std::getenv("TEST_DIR")) + "/";

    auto test(const std::string &command, const std::string &path) -> int {
        std::string buffer = command + " " + path + " &> " + tmp_file;
        return system(buffer.data());
    }

    // const std::string pattern1{"\'/\\w+options.c(p)*$\'"};
    const std::string pattern1{" zstd/.*doc/README[.]md$ "};

} // namespace

TEST_CASE("Find all files") {
    auto bench = ankerl::nanobench::Bench().minEpochIterations(10);
    SECTION("GNU find") {
        const std::string cmd = find_cmd;
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }

    SECTION("fd") {
        const std::string cmd = "fd -H --no-ignore .";
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }

    SECTION("fast-find (default)") {
        const std::string cmd = fast_find + " --donot-ignore-git";
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }

    SECTION("fast-find (bfs)") {
        const std::string cmd = fast_find + " --donot-ignore-git --bfs";
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }
}

TEST_CASE("Ignore the .git folder") {
    auto bench = ankerl::nanobench::Bench().minEpochIterations(10);

    SECTION("GNU find") {
        const std::string cmd = find_cmd;
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }

    SECTION("fd") {
        const std::string cmd = "fd .";
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }

    SECTION("fast-find (default)") {
        const std::string cmd = fast_find;
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }

    SECTION("fast-find (bfs)") {
        const std::string cmd = fast_find + " --bfs";
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }
}

TEST_CASE("Find with regex") {
    auto bench = ankerl::nanobench::Bench().minEpochIterations(10);
    SECTION("GNU find") {
        const std::string cmd = find_cmd + " -e " + pattern1;
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }

    SECTION("fd") {
        const std::string cmd = "fd " + pattern1;
        bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }

    SECTION("fast-find (default)") {
        const std::string cmd = fast_find + " -e " + pattern1;
    bench.run(cmd, [&cmd](){test(cmd, test_path);});
    }
}
