#include "celero/Celero.h"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <fstream>
#include <iostream>

#include "search.hpp"


void test_search(const char *path) {
    int fd = ::open(path, O_RDONLY);
    if (fd >= 0) {
        std::vector<std::string> p{path};
        ioutils::FileSearch<ioutils::StorePolicy> search;
        search.dfs(p);
    }
}

constexpr int number_of_samples = 10;
constexpr int number_of_operations = 1;


CELERO_MAIN

BASELINE(find, gnu_find, number_of_samples, number_of_operations) {
    system("find ../../3p/src/boost/ > /dev/null");
}

BENCHMARK(find, mfind, number_of_samples, number_of_operations) {
    test_search("../../3p/src/boost/");
}
