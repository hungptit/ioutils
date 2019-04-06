#include "clara.hpp"
#include "fmt/format.h"
#include "mlocate.hpp"
#include "search.hpp"
#include "utilities.hpp"
#include "utils/timeutils.hpp"
#include "writer.hpp"
#include <set>
#include <string>
#include <vector>

namespace {
    struct InputParams {
        std::vector<std::string> paths; // Input databases
        std::string database;
        bool stats = 0;
        bool verbose = 0;

        void print() const {
            fmt::print("Search paths: [\"{}\"]\n", fmt::join(paths, "\",\""));
            fmt::print("Database: {}\n", database);
            fmt::print("statistics: {}\n", stats);
            fmt::print("verbose: {}\n", verbose);
        }
    };

    InputParams parse_input_arguments(int argc, char *argv[]) {
        InputParams params;
        std::vector<std::string> paths;
        bool help = false;
        auto cli = clara::Help(help) |
                   clara::Opt(params.database, "database")["-d"]["--database"]("The file information database.") |
                   clara::Opt(params.verbose)["-v"]["--verbose"]("Display verbose information") |
                   clara::Arg(paths, "paths")("Search paths.");

        auto result = cli.parse(clara::Args(argc, argv));
        if (!result) {
            fmt::print(stderr, "Error in command line: {}\n", result.errorMessage());
            exit(1);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            fmt::print("{}", oss.str());
            exit(EXIT_SUCCESS);
        }

        if (params.database.empty()) {
            params.database = ".database";
        }

        if (paths.empty()) {
            fmt::print(stderr, "Must provide a search path.");
            exit(EXIT_FAILURE);
        }

        // Cleanup extra back slash characters.
        std::set<std::string> search_paths;
        for (auto p : paths) {
            ioutils::remove_trailing_slash(p);
            search_paths.emplace(p);
        }
        params.paths.insert(params.paths.begin(), search_paths.cbegin(), search_paths.cend());

        // Display input arguments in JSON format if verbose flag is on
        if (params.verbose) {
            params.print();
        }

        return params;
    }
} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    using Search = typename ioutils::FileSearch<ioutils::mlocate::Policy>;
    Search search;
    search.traverse(params.paths);

    // Serialize data to a string then save it to a file.
    fmt::memory_buffer buffer;
    auto const data = search.get_data();
    for (auto item : data) {
        fmt::format_to(buffer, "{0}\n", item.path);
    }
    if (params.verbose) fmt::print("Database size: {}\n", buffer.size());
    ioutils::write(buffer.data(), buffer.size(), params.database.data());

    return EXIT_SUCCESS;
}
