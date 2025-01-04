#include "catch2/catch_test_macros.hpp"
#include "ioutils/filesystem.hpp"
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

namespace {
    constexpr int warmup_times = 3;
    constexpr int minimum_iterations = 10;

    const std::string fname("filesystem.cpp");
    static struct stat buf;

    bool exist_stat(const std::string &p) {
        stat(p.data(), &buf);
        return true;
    };

    bool exist_open(const std::string &p) {
        int fd = open(fname.data(), O_RDONLY);
        bool result = fd > -1;
        close(fd);
        return result;
    }

    bool exist_access(const std::string &p) {
        return access(p.data(), R_OK) == 0;
    }
} // namespace

TEST_CASE("Check file exist algorithms") {
    auto bench = ankerl::nanobench::Bench().warmup(warmup_times).minEpochIterations(minimum_iterations);

    bench.run("Use stat", []() { exist_stat(fname); });
    bench.run("Use open", []() { exist_open(fname); });
    bench.run("Use access", []() { exist_access(fname); });
    bench.run("std::filesystem::exist", []() {
        return std::filesystem::exists(fname);
    });
}

TEST_CASE("Find stems") {
    const std::string stem("This");
    auto bench = ankerl::nanobench::Bench().warmup(warmup_times).minEpochIterations(minimum_iterations);
    bench.run("is_valid_dir", [&stem]() { ioutils::filesystem::is_valid_dir(stem.data()); });
    bench.run("is_valid_dir_slow", [&stem]() { ioutils::filesystem::is_valid_dir_slow(stem.data()); });
}
