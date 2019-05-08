#include "clara.hpp"
#include "fmt/format.h"
#include "locate.hpp"
#include "search.hpp"
#include "utilities.hpp"
#include "utils/timeutils.hpp"
#include "writer.hpp"
#include <set>
#include <string>
#include <vector>

namespace {
    void copyright() {
        fmt::print("{}\n", "fast-updatedb version 0.2.0");
        fmt::print("{}\n", "Copyright by Hung Dang <hungptit at gmail dot com>");
    }

    struct InputParams {
        int flags = 0;
        int maxdepth = std::numeric_limits<int>::max();
        std::vector<std::string> paths; // Input databases
        std::string database;
        bool stats = 0;
        bool verbose = 0;

        bool dfs() { return true; } // Use dfs traversal to explore folders.
        bool donot_ignore_git() const { return true; }
        bool follow_symlink() const { return false; }
        bool ignore_error() const { return false; }

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
        bool version = false;
        auto cli = clara::Help(help) |
                   clara::Opt(params.database, "database")["-d"]["--database"]("The file information database.") |
                   clara::Opt(params.verbose)["-v"]["--verbose"]("Display verbose information") |
                   clara::Opt(version)["--version"]("Display the fast-updatedb version") |
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
            copyright();
            exit(EXIT_SUCCESS);
        }

        if (version) {
            copyright();
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

        // Display input arguments if the verbose flag is on.
        if (params.verbose) {
            params.print();
        }

        return params;
    }
} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    using Policy = ioutils::locate::UpdateDBStreamPolicy;
    using Search = typename ioutils::filesystem::DefaultSearch<Policy>;
    Search search(params);
    search.traverse(params.paths);
    return EXIT_SUCCESS;
}
