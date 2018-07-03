#include "boost/program_options.hpp"
#include "fmt/format.h"
#include "regex_policies.hpp"
#include "search.hpp"
#include "search_policies.hpp"
#include "utilities.hpp"
#include "utils/matchers.hpp"
#include "utils/regex_matchers.hpp"

namespace {
    struct SearchParams {
        bool ignore_case;  // Ignore case distinctions
        bool invert_match; // Select non-matching lines
        bool verbose;      // Display verbose information.

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(ignore_case), CEREAL_NVP(invert_match), CEREAL_NVP(verbose));
        }
    };

    struct InputParams {
        std::string pattern;            // Input patterns
        std::vector<std::string> paths; // Input files and folders
        std::string output_file;        // Output file
        SearchParams parameters;

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(pattern), CEREAL_NVP(paths), CEREAL_NVP(output_file),
               CEREAL_NVP(parameters));
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

            ("ignore-case", "Ignore case.")
            ("invert-match", "Select non-matching files.")
            ("exact-match", "Use exact matching algorithms")

            ("pattern,r", po::value<std::string>(&params.pattern), "Search pattern")
            ("paths,p", po::value<std::vector<std::string>>(&paths), "A list of files and folders")
            ("output-file,o", po::value<std::string>(&params.output_file), "Output file");
        // clang-format on

        // Parse input arguments
        po::positional_options_description p;
        p.add("paths", -1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Pre-process input arguments
        if (vm.count("help")) {
            std::cout << desc << "\n";
            std::cout << "Examples:"
                      << "\n";
            std::cout << "    mfind ."
                      << "\n";
            std::cout << "    mfind -p '*.cpp' ."
                      << "\n";

            exit(EXIT_SUCCESS);
        }

        if (paths.empty()) {
            paths.emplace_back(".");
        }

        // Normalize paths
        for (auto p : paths) {
            ioutils::remove_trailing_slash(p);
            params.paths.emplace_back(p);
        }

        // Display input arguments in JSON format if verbose flag is on
        params.parameters.verbose = vm.count("verbose");
        params.parameters.ignore_case = vm.count("ignore-case");
        params.parameters.invert_match = vm.count("invert-match");

        if (params.parameters.verbose) {
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

    if (params.pattern.empty()) {
        using Search = typename ioutils::FileSearch<ioutils::ConsolePolicy>;
        Search search;
        search.dfs(params.paths);
    } else {
        // Compute mode using input parameter information.
        int mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
        if (params.parameters.ignore_case) {
            mode |= HS_FLAG_CASELESS;
        }

        // Find desired files and folders
        if (!params.parameters.invert_match) {
            using Matcher = utils::hyperscan::RegexMatcher;
            using Policy = ioutils::RegexPolicy<Matcher>;
            using Search = typename ioutils::FileSearch<Policy>;
            Search search(params.pattern, mode);
            search.dfs(params.paths);
        } else {
            using Matcher = utils::hyperscan::RegexMatcherInv;
            using Policy = ioutils::RegexPolicy<Matcher>;
            using Search = typename ioutils::FileSearch<Policy>;
            Search search(params.pattern, mode);
            search.dfs(params.paths);
        }
    }

    return EXIT_SUCCESS;
}
