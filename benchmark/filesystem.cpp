#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <benchmark/benchmark.h>

const std::string fname("filesystem.cpp");
struct stat buf;

// Use stat function
void use_stat(benchmark::State &state) {
    for (auto _ : state) {
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

BENCHMARK_MAIN();
