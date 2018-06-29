#include "fmt/format.h"
#include "mlocate.hpp"
#include "search.hpp"
#include "utilities.hpp"
#include "utils/timeutils.hpp"
#include "writer.hpp"

#include "boost/program_options.hpp"

namespace {
    struct InputParams {
        std::vector<std::string> paths; // Input databases
        std::string database;
        bool verbose;

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(paths), CEREAL_NVP(database), CEREAL_NVP(verbose));
        }
    };

    InputParams parse_input_arguments(int argc, char *argv[]) {
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        InputParams params;
        std::vector<std::string> paths;

        // clang-format off
        desc.add_options()
            ("help,h", "Print this help")
            ("verbose,v", "Display verbose information.")
            ("database,d", po::value<std::string>(&params.database), "An output file information database.")
            ("path,p", po::value<std::vector<std::string>>(&paths), "A list of folders");
        // clang-format on

        // Parse input arguments
        po::positional_options_description p;
        p.add("path", -1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Pre-process input arguments
        if (vm.count("help")) {
            std::cout << desc << "\n";
            std::cout << "Examples:"
                      << "\n";
            std::cout <<     "mupdatedb path1 path2 -d your_database_file "
                      << "\n";

            exit(EXIT_SUCCESS);
        }

        if (paths.empty()) {
            fmt::print(stderr, "Must provide a search path.");
            exit(EXIT_FAILURE);
        }

        // Cleanup extrace back slash characters.
        for (auto p : paths) {
            ioutils::remove_trailing_slash(p);
            params.paths.emplace_back(p);
        }

        if (params.database.empty()) {
            params.database = ".database";
        }

        // Display input arguments in JSON format if verbose flag is on
        params.verbose = vm.count("verbose");
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
