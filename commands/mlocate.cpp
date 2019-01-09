#include "mlocate.hpp"
#include "clara.hpp"
#include "filesystem.hpp"
#include "fmt/format.h"
#include "ioutils.hpp"
#include "utils/matchers.hpp"
#include "utils/regex_matchers.hpp"
#include <string>
#include "cereal/archives/json.hpp"

namespace {
    void copyright() {
        fmt::print("{}\n", "mlocate version 0.1.0");
        fmt::print("{}\n", "Hung Dang <hungptit@gmail.com>");
    }
    struct SearchParams {
        bool ignore_case = false;   // Ignore case distinctions
        bool inverse_match = false; // Select non-matching lines
        bool exact_match = false;   // Use exact matching algorithms.
        bool verbose = false;       // Display verbose information.
        bool info = false;
        int mode;           // Regex mode
        std::string prefix; // Root folder prefix

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(ignore_case), CEREAL_NVP(inverse_match), CEREAL_NVP(exact_match),
               CEREAL_NVP(verbose), CEREAL_NVP(info), CEREAL_NVP(prefix));
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
            clara::Opt(params.parameters.inverse_match)["-u"]["--inverse-match"](
                "Display lines that do not match given pattern.") |
            clara::Opt(params.parameters.exact_match)["-x"]["--exact-match"](
                "Use exact match algorithm") |
            clara::Opt(params.parameters.prefix, "prefix")["--prefix"]("Path prefix.") |
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
            copyright();
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

        // If user does not specify prefix then fallback to the
        // MLOCATE_PREFIX environment variable.
        if (params.parameters.prefix.empty()) {
            auto const default_prefix = std::getenv("MLOCATE_PREFIX");
            if (default_prefix) {
                params.parameters.prefix = std::string(default_prefix);
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

    template <typename Matcher, typename Params> void locate(Params &&params) {
        using GrepAlg = ioutils::MMapReader<ioutils::LocatePolicy<Matcher>>;
        GrepAlg grep(params);
        for (auto db : params.databases) {
            grep(db.data());
        }
    }
} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    for (auto db : params.databases) {
        if (params.pattern.empty()) {
            using GrepAlg = ioutils::MMapReader<ioutils::PrintAllPolicy>;
            GrepAlg grep(params);
            for (auto db : params.databases) {
                grep(db.data());
            }
        } else {
            params.parameters.mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
            if (params.parameters.ignore_case) {
                params.parameters.mode |= HS_FLAG_CASELESS;
            }

            if (!params.parameters.inverse_match) {
                using Matcher = utils::hyperscan::RegexMatcher;
                locate<Matcher>(params);
            } else {
                using Matcher = utils::hyperscan::RegexMatcherInv;
                locate<Matcher>(params);
            }
        }
    }
    return EXIT_SUCCESS;
}
