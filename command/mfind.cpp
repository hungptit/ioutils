#include "fmt/format.h"
#include "search.hpp"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return EXIT_FAILURE;
    }

    std::vector<std::string> p;
    for (auto idx = argc - 1; idx > 0; --idx) {
        int fd = ::open(argv[idx], O_RDONLY);
        if (fd >= 0) {
            p.emplace_back(argv[idx]);
        }
        ioutils::FileSearch search;
        search.dfs(p);
    }

    return EXIT_SUCCESS;
}
