#include "celero/Celero.h"
#include <iostream>

constexpr int number_of_samples = 10;
constexpr int number_of_operations = 1;

#ifdef __APPLE__
const std::string find_cmd = "gfind ";
#else
const std::string find_cmd = "find ";
#endif

const std::string fast_find("../commands/fast-find ");
const std::string tmp_file = "/tmp/output.log";
const std::string test_path(std::getenv("TEST_DIR"));

auto test(const std::string &command, const std::string &path) -> int {
    std::string buffer = command + path + " &> " + tmp_file;
    return system(buffer.data());
}

auto test_find_regex(const std::string &command, const std::string &regex, const std::string &path) -> int {
    std::string buffer = command + " " + path + " |& grep -E " + regex + " > " + tmp_file;
    return system(buffer.data());
}

auto test_fast_find_regex(const std::string &command, const std::string &regex, const std::string &path) -> int {
    std::string buffer = command + " -e " + regex + " " + path + " &> " + tmp_file;
    return system(buffer.data());
}

auto test_fd_regex(const std::string &command, const std::string &regex, const std::string &path) -> int {
    std::string buffer = command + " " + regex + " " + path + " &> " + tmp_file;
    return system(buffer.data());
}

// const std::string pattern1{"\'/\\w+options.c(p)*$\'"};
const std::string pattern1{" zstd/.*doc/README[.]md$ "};

CELERO_MAIN

// Find all files in mid size folder.
BASELINE(all, find, number_of_samples, number_of_operations) { test(find_cmd, test_path); }

BENCHMARK(all, fd_noignore, number_of_samples, number_of_operations) { test("fd -H --no-ignore . ", test_path); }

BENCHMARK(all, fast_find, number_of_samples, number_of_operations) {
    test(fast_find + " --donot-ignore-git ", test_path);
}

BENCHMARK(all, fast_find_bfs, number_of_samples, number_of_operations) {
    test(fast_find + " --bfs --donot-ignore-git ", test_path);
}

// Find all files and ignore .git folder
BASELINE(ignore_git, find, number_of_samples, number_of_operations) { test(find_cmd, test_path); }

BENCHMARK(ignore_git, fd, number_of_samples, number_of_operations) { test("fd  . ", test_path); }

BENCHMARK(ignore_git, fast_find_default, number_of_samples, number_of_operations) { test(fast_find, test_path); }

BENCHMARK(ignore_git, fast_find_bfs, number_of_samples, number_of_operations) {
    test(fast_find + " --bfs ", test_path);
}

// Find all files using a regex that does not match any results
BASELINE(regex, find, number_of_samples, number_of_operations) { test_find_regex(find_cmd, pattern1, test_path); }

BENCHMARK(regex, fd, number_of_samples, number_of_operations) { test_fd_regex("fd --full-path ", pattern1, test_path); }

BENCHMARK(regex, fast_find, number_of_samples, number_of_operations) {
    test_fast_find_regex(fast_find, pattern1, test_path);
}

BENCHMARK(regex, fast_find_bfs, number_of_samples, number_of_operations) {
    test_fast_find_regex(fast_find + " --bfs", pattern1, test_path);
}
