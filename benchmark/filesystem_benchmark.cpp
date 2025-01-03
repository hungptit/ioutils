#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "ioutils/filesystem.hpp"
#include <filesystem>
#include <benchmark/benchmark.h>

const std::string fname("filesystem.cpp");
static struct stat buf;

// Use stat function
void use_stat(benchmark::State &state) {
    for (auto _ : state) {
        struct stat buf;
        benchmark::DoNotOptimize(stat(fname.data(), &buf));
    }
}
BENCHMARK(use_stat);

// Use open function
void use_open(benchmark::State &state) {
    for (auto _ : state) {
        int fd = open(fname.data(), O_RDONLY);
        if (fd > -1) close(fd);
    }
}
BENCHMARK(use_open);

// Use access function
void use_access(benchmark::State &state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(access(fname.data(), R_OK));
    }
}
BENCHMARK(use_access);

void use_std(benchmark::State &state) {
    for (auto _ : state) {
        std::filesystem::path p(fname);
        benchmark::DoNotOptimize(std::filesystem::exists(p));
    }
}
BENCHMARK(use_std);

const std::string stem("This");

// Simple if
void use_simple_if(benchmark::State &state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(ioutils::filesystem::is_valid_dir_slow(stem.data()));
    }
}
BENCHMARK(use_simple_if);

void use_recursive(benchmark::State &state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(ioutils::filesystem::is_valid_dir(stem.data()));
    }
}
BENCHMARK(use_recursive);

BENCHMARK_MAIN();
