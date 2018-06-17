#include "fmt/format.h"
#include "search.hpp"
#include "search_policies.hpp"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> p;
    for (auto idx = argc - 1; idx > 0; --idx) {
        p.emplace_back(argv[idx]);
        // using Search = typename ioutils::FileSearch<ioutils::ConsolePolicy>;
        using Search = typename ioutils::FileSearch<ioutils::StorePolicy>;
        Search search;
        search.dfs(p);
        fmt::print("Number of files: {}\n", search.get_files().size());
    }
    return EXIT_SUCCESS;
}
