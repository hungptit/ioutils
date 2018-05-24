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

CELERO_MAIN

BASELINE(find, gnu_find, number_of_samples, number_of_operations) {
    system("find ../../3p/src/boost/ > /dev/null");
}

BENCHMARK(find, mfind_to_console, number_of_samples, number_of_operations) {
    system("../command/mfind ../../3p/src/boost/ > /dev/null");
}

BENCHMARK(find, mfind_dfs_store, number_of_samples, number_of_operations) {
    test_search_dfs("../../3p/src/boost/");
}

BENCHMARK(find, mfind_bfs_store, number_of_samples, number_of_operations) {
    test_search_bfs("../../3p/src/boost/");
}
