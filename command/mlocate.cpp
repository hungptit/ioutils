#include "mlocate.hpp"
#include "boost/program_options.hpp"
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
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        InputParams params;
        std::vector<std::string> db, paths;

        // clang-format off
        desc.add_options()
            ("help,h", "Print this help")
            ("verbose,v", "Display verbose information.")
            ("info", "Display database information.")
            ("ignore-case", "Ignore case.")
            ("exact-match", "Use exact matching algorithms")
            ("invert-match", "Select non-matching lines.")
            ("pattern,e", po::value<std::string>(&params.pattern), "Search pattern")
            ("database,d", po::value<std::vector<std::string>>(&db), "A list of mlocate database");
        // clang-format on

        // Parse input arguments
        po::positional_options_description p;
        p.add("pattern", -1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        // Pre-process input arguments
        if (vm.count("help")) {
            std::cout << desc << "\n";
            std::cout << "Examples:"
                      << "\n";
            std::cout << "    mlocate foo"
                      << "\n";
            std::cout << "    mlocate -p '*.cpp' -d foo.bin"
                      << "\n";

            exit(EXIT_SUCCESS);
        }

        if (db.empty()) {
            auto default_db = std::getenv("MLOCATE_DB");
            if (default_db == nullptr) {
                params.databases.emplace_back(".database");
            } else {
                params.databases.push_back(default_db);
            }
        } else {
            for (auto p : db) {
                params.databases.push_back(p);
            }
        }

        // Display input arguments in JSON format if verbose flag is on
        params.parameters.verbose = vm.count("verbose");
        params.parameters.ignore_case = vm.count("ignore-case");
        params.parameters.invert_match = vm.count("invert-match");
        params.parameters.exact_match = vm.count("exact-match");
        params.parameters.info = vm.count("info");

        if (params.parameters.verbose) {
            params.parameters.verbose = true;
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
            fmt::print(stderr, "Cannot read data from {0} or the file content is empty.\n", db);
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
