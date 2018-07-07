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

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(paths), CEREAL_NVP(database), CEREAL_NVP(verbose));
        }
    };

    InputParams parse_input_arguments(int argc, char *argv[]) {
        InputParams params;
        std::vector<std::string> paths;
        bool help = false;
        auto cli =
            clara::Help(help) |
            clara::Opt(params.database,
                       "database")["-d"]["--database"]("The file information database.") |
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
            std::stringstream ss;
            {
                cereal::JSONOutputArchive ar(ss);
                ar(cereal::make_nvp("Input arguments", params));
            }
            fmt::print("{}\n", ss.str());
        }

        return params;
    }
} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    using Search = typename ioutils::FileSearch<ioutils::mlocate::Policy>;
    Search search;
    search.dfs(params.paths);

    // Serialize data to a string then save it to a file.
    std::string buffer = ioutils::save<cereal::BinaryOutputArchive>(search.get_data());
    if (params.verbose) fmt::print("buffer size: {}\n", buffer.size());
    ioutils::write(buffer.data(), buffer.size(), params.database.data());

    return EXIT_SUCCESS;
}
