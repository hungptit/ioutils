#include "celero/Celero.h"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <fstream>
#include <iostream>

#include "search.hpp"
#include "search_policies.hpp"
#include "simple_store_policy.hpp"

constexpr int number_of_samples = 20;
constexpr int number_of_operations = 1;

int test(const std::string &command, const std::string &path) {
    std::string buffer = command + path + " > /tmp/output.log";
    return system(buffer.data());
}

int test_find_regex(const std::string &command, const std::string &regex,
                    const std::string &path) {
    std::string buffer = command + " " + path + " | grep -E " + regex + " > /tmp/output.log";
    return system(buffer.data());
}

int test_fast_find_regex(const std::string &command, const std::string &regex,
                     const std::string &path) {
    std::string buffer = command + " -e " + regex + " " + path + " > /tmp/output.log";
    return system(buffer.data());
}

int test_fd_regex(const std::string &command, const std::string &regex,
                  const std::string &path) {
    std::string buffer = command + "." + path + " | grep -E " + regex + "> /tmp/output.log";
    return system(buffer.data());
}

const std::string pattern1{"\'/\\w+options.c(p)*$\'"};

CELERO_MAIN

// Find all files in the boost source code
BASELINE(big_folder, gnu_find, number_of_samples, number_of_operations) {
    test("gfind ", "../../3p/src/boost");
}

BENCHMARK(big_folder, fd, number_of_samples, number_of_operations) {
    test("fd  . ", "../../3p/src/boost");
}

BENCHMARK(big_folder, fd_noignore, number_of_samples, number_of_operations) {
    test("fd --no-ignore . ", "../../3p/src/boost");
}

BENCHMARK(big_folder, fast_find_default, number_of_samples, number_of_operations) {
    test("../commands/fast-find ", "../../3p/src/boost");
}

BENCHMARK(big_folder, fast_find_bfs, number_of_samples, number_of_operations) {
    test("../commands/fast-find --bfs ", "../../3p/src/boost");
}

BENCHMARK(big_folder, fast_find_default_donot_ignore_git, number_of_samples, number_of_operations) {
    test("../commands/fast-find --donot-ignore-git ", "../../3p/src/boost");
}

BENCHMARK(big_folder, fast_find_bfs_donot_ignore_git, number_of_samples, number_of_operations) {
    test("../commands/fast-find --bfs --donot-ignore-git ", "../../3p/src/boost");
}


// Find all files using a regex that does not match any results
BASELINE(big_folder_regex, gnu_find, number_of_samples, number_of_operations) {
    test_find_regex("find ", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, fd, number_of_samples, number_of_operations) {
    test_fd_regex("fd ", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, fd_noignore, number_of_samples, number_of_operations) {
    test_fd_regex("fd --no-ignore ", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, fast_find_default, number_of_samples, number_of_operations) {
    test_fast_find_regex("../commands/fast-find ", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, fast_find_bfs, number_of_samples, number_of_operations) {
    test_fast_find_regex("../commands/fast-find --bfs", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, fast_find_default_donot_ignore_git, number_of_samples, number_of_operations) {
    test_fast_find_regex("../commands/fast-find --donot-ignore-git ", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, fast_find_bfs_donot_ignore_git, number_of_samples, number_of_operations) {
    test_fast_find_regex("../commands/fast-find --bfs --donot-ignore-git ", pattern1, " ../../3p/src/boost");
}
