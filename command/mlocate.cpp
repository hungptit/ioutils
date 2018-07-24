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
        bool ignore_case = false;  // Ignore case distinctions
        bool invert_match = false; // Select non-matching lines
        bool exact_match = false;  // Use exact matching algorithms.
        bool verbose = false;      // Display verbose information.
        bool info = false;

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
} // namespace

int main(int argc, char *argv[]) {
    using container_type = ioutils::mlocate::Policy::container_type;
    auto params = parse_input_arguments(argc, argv);
    for (auto db : params.databases) {
        std::string buffer = ioutils::read(db.data());
        if (buffer.empty()) {
            if (params.parameters.verbose) {
                fmt::print(stderr, "Cannot read data from {0} or the file content is empty.\n",
                           db);
            }
            continue;
        }

        if (params.parameters.verbose) {
            fmt::print("Read bytes: {}\n", buffer.size());
        }

        container_type data =
            ioutils::load<cereal::BinaryInputArchive, container_type>(std::move(buffer));

        if (params.parameters.info) {
            fmt::print("The number of files and folders: {}\n", data.size());
            continue;
        }

        if (params.pattern.empty()) {
            // Display all files
            for (auto &item : data) {
                fmt::print("{0}\n", item.path);
            }
        } else {
            // Only display files that match the given pattern.
            if (params.parameters.exact_match) {
                utils::ExactMatchSSE2 matcher(
                    params.pattern.data()); // Use exact matching algorithm.
                for (auto &item : data) {
                    if (matcher.is_matched(item.path.data(), item.path.size())) {
                        fmt::print("{0}\n", item.path);
                    }
                }
            } else {
                // Use regular expression engine. This approach will
                // slightly slower than that of exact matching
                // algorithm.
                int mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
                if (params.parameters.ignore_case) {
                    mode |= HS_FLAG_CASELESS;
                }

                if (!params.parameters.invert_match) {
                    utils::hyperscan::RegexMatcher matcher(params.pattern.data(), mode);
                    for (auto &item : data) {
                        if (matcher.is_matched(item.path.data(), item.path.size())) {
                            fmt::print("{0}\n", item.path);
                        }
                    }

                } else {
                    utils::hyperscan::RegexMatcherInv matcher(params.pattern.data(), mode);
                    for (auto &item : data) {
                        if (matcher.is_matched(item.path.data(), item.path.size())) {
                            fmt::print("{0}\n", item.path);
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
