#include "fmt/format.h"
#include "search.hpp"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> p;
    for (auto idx = argc - 1; idx > 0; --idx) {
        p.emplace_back(argv[idx]);
        ioutils::FileSearch search;
        search.bfs(p);
    }
    return EXIT_SUCCESS;
}
