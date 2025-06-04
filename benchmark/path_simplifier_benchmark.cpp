#include "catch2/catch_test_macros.hpp"
#include "ioutils/utilities.hpp"
#include <string>
#include <vector>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

namespace {
    constexpr int WARMUP_TIMES = 3;
    constexpr int MIN_ITERATIONS = 100000;

    static const std::vector<std::string> test_paths = {
        "_deps/boost-src/libs/outcome/include/boost/outcome/detail/coroutine_support.ipp",
        "/home/hungptit/working/ioutils/_deps/boost-src/libs/outcome/include/boost/outcome/detail/coroutine_support.ipp",
        "_deps/boost-src/libs/../../outcome/include/boost/../outcome/detail/coroutine_support.ipp",
    };

    void run_tests(const std::vector<std::string> &test_paths) {}
} // namespace

TEST_CASE("Basic benchmark") {
    auto bm = ankerl::nanobench::Bench().warmup(WARMUP_TIMES).minEpochIterations(MIN_ITERATIONS);
    bm.run("Path beautifier", []() {
        for (const auto &p : test_paths) {
            ankerl::nanobench::doNotOptimizeAway(ioutils::path::simplify_path(p));
        }
    });
}
