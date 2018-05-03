#include "celero/Celero.h"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <fstream>
#include <iostream>

#include "search.hpp"


void test_search(const char *path) {
    int fd = ::open(path, O_RDONLY);
    if (fd >= 0) {
        std::vector<ioutils::Path> p{{fd, path}};
        ioutils::FileSearch search;
        search.dfs(p);
    }
}

constexpr int number_of_samples = 10;
constexpr int number_of_operations = 1;


CELERO_MAIN

BASELINE(find, test_search, number_of_samples, number_of_operations) {
    test_search("/Users/hdang/working/3p/src/boost/");
}
