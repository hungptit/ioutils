#include "clara.hpp"
#include "fmt/format.h"

#include "ioutils/search.hpp"
#include "ioutils/updatedb.hpp"
#include "ioutils/utilities.hpp"
#include "ioutils/writer.hpp"
#include "version.hpp"

#include "timeutils.hpp"

#include <set>
#include <string>
#include <vector>

namespace {
    void updatedb_disp_version() { fmt::print("{} {}\n", "fast-updatedb ", version); }
    void updatedb_usage() {
        fmt::print("\nExamples:\n");
        fmt::print("\t1. Build a database for all files from given paths:\n");
        fmt::print("\t\tfast-updatedb -d my_db /\n");
        fmt::print("\t2. Build the file information database for given paths and save to the default path :\n");
        fmt::print("\t\tfast-updatedb ../src/\n");
    }

    auto parse_input_arguments(int argc, char *argv[]) {
        ioutils::UpdateDBInputArguments params;
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
            updatedb_usage();
            exit(1);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            fmt::print("{}", oss.str());
            updatedb_usage();
            ioutils::copyright();
            exit(EXIT_SUCCESS);
        }

        if (version) {
            updatedb_disp_version();
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
            search_paths.emplace(ioutils::path::simplify_path(p));
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
    ioutils::updatedb(params);
    return EXIT_SUCCESS;
}
