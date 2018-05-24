#include "fmt/format.h"
#include "mlocate.hpp"
#include "search.hpp"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> p;
    for (auto idx = argc - 1; idx > 0; --idx) {
        p.emplace_back(argv[idx]);
    }

    using Search = typename ioutils::FileSearch<ioutils::mlocate::MlocatePolicy>;
    Search search;
    search.dfs(p);
    search.print();

    return EXIT_SUCCESS;
}
