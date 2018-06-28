#include "celero/Celero.h"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <fstream>
#include <iostream>

#include "search.hpp"
#include "search_policies.hpp"

void test_search_dfs(const char *path) {
    int fd = ::open(path, O_RDONLY);
    if (fd < 0) throw "Cannot open test folder";
    std::vector<std::string> p{path};
    ioutils::FileSearch<ioutils::StorePolicy> search;
    search.dfs(p);
}

void test_search_bfs(const char *path) {
    int fd = ::open(path, O_RDONLY);
    if (fd < 0) throw "Cannot open a test folder!";
    std::vector<std::string> p{path};
    ioutils::FileSearch<ioutils::StorePolicy> search;
    search.bfs(p);
}

constexpr int number_of_samples = 10;
constexpr int number_of_operations = 1;

void test(const std::string &command, const std::string &path) {
    std::string buffer = command + path + " > /tmp/output.log";
    system(buffer.data());
}

void test_find_regex(const std::string &command, const std::string &regex, const std::string &path) {
    std::string buffer = command + " " + path + " | grep " + regex + " > /tmp/output.log";
    system(buffer.data());
}

void test_mfind_regex(const std::string &command, const std::string &regex, const std::string &path) {
    std::string buffer = command + " -r " + regex + " " + path + " > /tmp/output.log";
    system(buffer.data());
}

void test_fd_regex(const std::string &command, const std::string &regex, const std::string &path) {
    std::string buffer = command + "." + path + " | grep " + regex + "> /tmp/output.log";
    system(buffer.data());
}

const std::string pattern1{"\'/\\w+options.c(p)*$\'"};

CELERO_MAIN

// Find all files in the boost source code
BASELINE(boost, gnu_find, number_of_samples, number_of_operations) {
    test("find ", "../../3p/src/boost/");
}

BENCHMARK(boost, fd, number_of_samples, number_of_operations) {
    test("fd --no-ignore . ", "../../3p/src/boost/");
}

BENCHMARK(boost, mfind_to_console, number_of_samples, number_of_operations) {
    test("../command/mfind ", "../../3p/src/boost/");
}

// Find all files in the kernel source code.
BASELINE(linux_kernel, gnu_find, number_of_samples, number_of_operations) {
    test("find ", "/usr/src/linux-4.17.1-gentoo/");
}

BENCHMARK(linux_kernel, fd, number_of_samples, number_of_operations) {
    test("fd --no-ignore . ", "/usr/src/linux-4.17.1-gentoo/");
}

BENCHMARK(linux_kernel, mfind_to_console, number_of_samples, number_of_operations) {
    test("../command/mfind ", "/usr/src/linux-4.17.1-gentoo/");
}

// Find all files using a regex that does not match any results
BASELINE(boost_regex, gnu_find, number_of_samples, number_of_operations) {
    test_find_regex("find ", pattern1, " ../../3p/src/boost/");
}

BENCHMARK(boost_regex, fd, number_of_samples, number_of_operations) {
    test_fd_regex("fd --no-ignore ", pattern1, " ../../3p/src/boost/");
}

BENCHMARK(boost_regex, mfind_to_console, number_of_samples, number_of_operations) {
    test_mfind_regex("../command/mfind ", pattern1, " ../../3p/src/boost/");
}

// Find all files using a regex that matches some files.


// BENCHMARK(boost, mfind_dfs_store, number_of_samples, number_of_operations) {
//     test_search_dfs("../../3p/src/boost/");
// }

// BENCHMARK(boost, mfind_bfs_store, number_of_samples, number_of_operations) {
//     test_search_bfs("../../3p/src/boost/");
// }
