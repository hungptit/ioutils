#include "clara.hpp"
#include "fmt/format.h"
#include "regex_policies.hpp"
#include "search.hpp"
#include "search_policies.hpp"
#include "simple_policy.hpp"
#include "utilities.hpp"
#include "utils/matchers.hpp"
#include "utils/regex_matchers.hpp"
#include <dirent.h>
#include "cereal/archives/json.hpp"

namespace {
    struct SearchParams {
        bool inverse_match = false;                 // Select non-matching lines
        bool verbose = false;                       // Display verbose information.
        int type = ioutils::DisplayType::DISP_NONE; // Display type
        int regex_mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(inverse_match), CEREAL_NVP(verbose), CEREAL_NVP(type),
               CEREAL_NVP(regex_mode));
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
        bool ignore_case = false;
        bool ignore_dir = false;
        bool ignore_file = false;
        bool ignore_symlink = false;
        bool color = false;

        bool follow_link = false;

        std::string begin_time, end_time;

        auto cli =
            clara::Help(help) |
            clara::Opt(params.parameters.verbose)["-v"]["--verbose"](
                "Display verbose information") |
            clara::Opt(ignore_case)["-i"]["--ignore-case"]("Ignore case") |
            clara::Opt(params.parameters.inverse_match)["-u"]["--inverse-match"](
                "Select lines that do not match specified pattern.") |
            clara::Opt(ignore_file)["--ignore-file"]("Ignore files.") |
            clara::Opt(ignore_dir)["--ignore-dir"]("Ignore folders.") |
            clara::Opt(ignore_symlink)["--ignore-symlink"]("Ignore symlink.") |
            clara::Opt(color)["-c"]["--color"]("Print out color text.") |
            clara::Opt(params.pattern, "pattern")["-e"]["--pattern"]("Search pattern.") |

            // Unsupported options
            clara::Opt(follow_link, "follow-link")["--follow-link"]("Follow symbolic links.") |
            clara::Opt(begin_time, "newer")["--newer"]("Follow symbolic links.") |
            clara::Opt(end_time, "older")["--older"]("Follow symbolic links.") |

            // Required arguments.
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

        // Init the mode for our regular expression engine.
        if (ignore_case) {
            params.parameters.regex_mode |= HS_FLAG_CASELESS;
        }

        // Parse the display type
        params.parameters.type = !ignore_file * ioutils::DisplayType::DISP_FILE +
                                 !ignore_dir * ioutils::DisplayType::DISP_DIR +
                                 !ignore_symlink * ioutils::DisplayType::DISP_SYMLINK +
                                 color * ioutils::DisplayType::DISP_COLOR;

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

    template <typename Policy, typename Params> void search(Params &&params) {
        using Search = typename ioutils::FileSearch<Policy>;
        Search search(params.pattern, params.parameters);
        search.dfs(params.paths);
    }
} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);

    // Search for files based on given constraints.
    if (params.pattern.empty()) {
        using Policy = ioutils::SimplePolicy<decltype(params.parameters)>;
        search<Policy>(params);
    } else {
        // Find desired files and folders
        if (!params.parameters.inverse_match) {
            using Matcher = utils::hyperscan::RegexMatcher;
            using Policy = ioutils::RegexPolicy<Matcher, decltype(params.parameters)>;
            search<Policy>(params);
        } else {
            using Matcher = utils::hyperscan::RegexMatcherInv;
            using Policy = ioutils::RegexPolicy<Matcher, decltype(params.parameters)>;
            search<Policy>(params);
        }
    }

    return EXIT_SUCCESS;
}
