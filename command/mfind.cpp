#include "clara.hpp"
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
        InputParams params;
        std::vector<std::string> paths;
        bool help = false;
        auto cli =
            clara::Help(help) |
            clara::Opt(params.parameters.verbose)["-v"]["--verbose"](
                "Display verbose information") |
            clara::Opt(params.parameters.ignore_case)["-i"]["--ignore-case"](
                "Ignore case") |
            clara::Opt(params.parameters.invert_match)["-u"]["--invert-match"](
                "Select lines that do not match specified pattern.") |
            clara::Opt(params.pattern,
                       "pattern")["-e"]["--pattern"]("Search pattern.") |
            clara::Arg(paths, "paths")("Search paths");

        auto result = cli.parse(clara::Args(argc, argv));
        if (!result) {
            fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
            exit(1);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            fmt::print("{}", oss.str());
            exit(EXIT_SUCCESS);
        }

        if (paths.empty()) {
            params.paths.push_back(".");
        } else {
            for (auto p : paths) {
                ioutils::remove_trailing_slash(p);
                params.paths.emplace_back(p);
            }
        }

        // Display input arguments in JSON format if verbose flag is on
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
