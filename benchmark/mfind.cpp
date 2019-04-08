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

int test_mfind_regex(const std::string &command, const std::string &regex,
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

BENCHMARK(big_folder, mfind_default, number_of_samples, number_of_operations) {
    test("../commands/mfind ", "../../3p/src/boost");
}

BENCHMARK(big_folder, mfind_dfs, number_of_samples, number_of_operations) {
    test("../commands/mfind --dfs ", "../../3p/src/boost");
}

// Find all files using a regex that does not match any results
BASELINE(big_folder_regex, gnu_find, number_of_samples, number_of_operations) {
    test_find_regex("find ", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, fd, number_of_samples, number_of_operations) {
    test_fd_regex("fd ", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, mfind_default, number_of_samples, number_of_operations) {
    test_mfind_regex("../commands/mfind ", pattern1, " ../../3p/src/boost");
}

BENCHMARK(big_folder_regex, mfind_dfs, number_of_samples, number_of_operations) {
    test_mfind_regex("../commands/mfind --dfs", pattern1, " ../../3p/src/boost");
}
