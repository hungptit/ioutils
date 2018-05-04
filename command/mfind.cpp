#include "fmt/format.h"
#include "search.hpp"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return EXIT_FAILURE;
    }

    std::vector<ioutils::Path> p;
    for (auto idx = 1; idx < argc; ++idx) {
        int fd = ::open(argv[idx], O_RDONLY);
        if (fd >= 0) {
            p.emplace_back(ioutils::Path(fd, std::string(argv[idx])));
        }
        ioutils::FileSearch search;
        search.dfs(p);
    }

    return EXIT_SUCCESS;
}
