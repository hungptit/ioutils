#include "fmt/format.h"
#include "mlocate.hpp"
#include "search.hpp"
#include "utils/timeutils.hpp"
#include "utilities.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::string> p;
    for (auto idx = argc - 1; idx > 0; --idx) {
		std::string path(argv[idx]);
		ioutils::remove_trailing_slash(path);
        p.emplace_back(path);
    }
    using Search = typename ioutils::FileSearch<ioutils::mlocate::MlocatePolicy>;
    Search search;
    search.dfs(p);
    search.print();

    return EXIT_SUCCESS;
}
