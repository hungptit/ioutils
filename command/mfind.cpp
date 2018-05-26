#include "fmt/format.h"
#include "search.hpp"
#include "search_policies.hpp"
#include "utilities.hpp"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> p;
    for (auto idx = argc - 1; idx > 0; --idx) {
        std::string path(argv[idx]);
        ioutils::remove_trailing_slash(path);
        p.emplace_back(path);
    }

    using Search = typename ioutils::FileSearch<ioutils::ConsolePolicy>;
    // using Search = typename ioutils::FileSearch<ioutils::StorePolicy>;
    Search search;
    search.dfs(p);
    return EXIT_SUCCESS;
}
