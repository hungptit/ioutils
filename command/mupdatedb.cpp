#include "fmt/format.h"
#include "mlocate.hpp"
#include "search.hpp"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> p;
    for (auto idx = argc - 1; idx > 0; --idx) {
        p.emplace_back(argv[idx]);
    }

    using Search = typename ioutils::FileSearch<ioutils::MlocatePolicy>;
    Search search;
    search.dfs(p);
    // fmt::print("Number of files: {}\n", search.get_files().size());

    return EXIT_SUCCESS;
}
