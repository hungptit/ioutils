#include "mlocate.hpp"
#include "clara.hpp"
#include "filesystem.hpp"
#include "fmt/format.h"
#include "ioutils.hpp"
#include "utils/matchers.hpp"
#include "utils/regex_matchers.hpp"
#include <string>

namespace {
    struct SearchParams {
        bool ignore_case;  // Ignore case distinctions
        bool invert_match; // Select non-matching lines
        bool exact_match;  // Use exact matching algorithms.
        bool verbose;      // Display verbose information.
        bool info;

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(ignore_case), CEREAL_NVP(invert_match), CEREAL_NVP(exact_match),
               CEREAL_NVP(verbose), CEREAL_NVP(info));
        }
    };

    struct InputParams {
        std::string pattern;                // Input patterns
        std::vector<std::string> databases; // Input databases
        SearchParams parameters;

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(pattern), CEREAL_NVP(databases), CEREAL_NVP(parameters));
        }
    };

    InputParams parse_input_arguments(int argc, char *argv[]) {
        InputParams params;
        bool help = false;
        auto cli =
            clara::Help(help) |
            clara::Opt(params.databases,
                       "database")["-d"]["--database"]("The file information database.") |
            clara::Opt(params.parameters.verbose)["-v"]["--verbose"](
                "Display verbose information") |
            clara::Opt(params.parameters.ignore_case)["-i"]["--ignore-case"]("Ignore case.") |
            clara::Opt(params.parameters.invert_match)["-u"]["--invert-match"](
                "Display lines that do not match given pattern.") |
            clara::Opt(params.parameters.exact_match)["-x"]["--exact-match"](
                "Use exact match algorithm") |
            clara::Opt(params.parameters.info)["--info"]("Display database information") |
            clara::Arg(params.pattern, "pattern")("Search pattern");

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

        if (params.databases.empty()) {
            auto default_db = std::getenv("MLOCATE_DB");
            if (default_db == nullptr) {
                params.databases.emplace_back(".database");
            } else {
                params.databases.push_back(default_db);
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

    // Find the matched file names from the database.
    template <typename T> void fgrep(const InputParams &params) {
        T grep(params.pattern.data());
        for (auto afile : params.databases) {
            grep(afile.data());
        }
    }

    template <typename Matcher, typename Params> void locate(Params &&params) {
        using GrepAlg = ioutils::MMapReader<ioutils::LocatePolicy<Matcher>>;
        GrepAlg grep(params.pattern.data());
        for (auto db : params.databases) {
            grep(db.data());
        }
    }

} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    for (auto db : params.databases) {
        if (params.pattern.empty()) {
            // TODO: Update this
        } else {
            // Only display files that match the given pattern.
            if (params.parameters.exact_match) {
                using Matcher = utils::ExactMatchSSE2;
                locate<Matcher>(params);
            } else {
                // Use regular expression engine. This approach will
                // slightly slower than using the exact matching
                // algorithm.
                int mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
                if (params.parameters.ignore_case) {
                    mode |= HS_FLAG_CASELESS;
                }

                if (!params.parameters.invert_match) {
                    using Matcher = utils::hyperscan::RegexMatcher;
                    locate<Matcher>(params);
                } else {
                    using Matcher = utils::hyperscan::RegexMatcherInv;
                    locate<Matcher>(params);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
