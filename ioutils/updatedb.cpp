#include "ioutils/updatedb.hpp"
#include "ioutils/search.hpp"

#include "fmt/format.h"

namespace ioutils {

    void UpdateDBInputArguments::print() const {
        fmt::print("verbose: {}\n", verbose);
        fmt::print("statistics: {}\n", stats);
        fmt::print("Search paths: [\"{}\"]\n", fmt::join(paths, "\",\""));
        fmt::print("Database: {}\n", database);
    }

    void updatedb(ioutils::UpdateDBInputArguments &params) {
        using Policy = ioutils::UpdateDBStreamPolicy;
        using Search = typename ioutils::filesystem::DefaultSearch<Policy>;
        Search search(params);
        search.traverse(params.paths);
    }
} // namespace ioutils
