#include "clara.hpp"
#include "filesystem.hpp"
#include "fmt/format.h"
#include "ioutils.hpp"
#include "locate.hpp"
#include "utils/matchers.hpp"
#include "utils/memchr.hpp"
#include "utils/regex_matchers.hpp"
#include <string>

namespace {
    void disp_version() { fmt::print("{}\n", "fast-locate version 1.0"); }
    void copyright() { fmt::print("{}\n", "Copyright by Hung Dang <hungptit at gmail dot com>"); }

    enum PARAMS : uint32_t {
        VERBOSE = 1,
        COLOR = 1 << 1,
        INVERT_MATCH = 1 << 2,
        EXACT_MATCH = 1 << 3,
        IGNORE_CASE = 1 << 4,
        INFO = 1 << 5,
        COUNT = 1 << 6,
    };

    struct InputParams {
        int flags;
        int regex_mode;
        std::string prefix;                 // Prefix of displayed path
        std::string pattern;                // A search pattern
        std::vector<std::string> databases; // A file information database

        bool verbose() const { return (flags & VERBOSE) > 0; }
        bool info() const { return (flags & INFO) > 0; }
        bool invert_match() const { return (flags & INVERT_MATCH) > 0; }
        bool exact_match() const { return (flags & EXACT_MATCH) > 0; }
        bool ignore_case() const { return (flags & IGNORE_CASE) > 0; }

        void print() const {
            fmt::print("verbose: {}\n", verbose());
            fmt::print("info: {}\n", info());
            fmt::print("invert-match: {}\n", invert_match());
            fmt::print("exact-match: {}\n", exact_match());
            fmt::print("ignore-case: {}\n", ignore_case());
            fmt::print("regex-mode: {}\n", regex_mode);

            fmt::print("Search pattern: '{}'\n", pattern);
            fmt::print("path prefix: '{}'\n", prefix);
            fmt::print("File information databases: [\"{}\"]\n", fmt::join(databases, "\",\""));
        }
    };

    InputParams parse_input_arguments(int argc, char *argv[]) {
        InputParams params;

        bool version = false;
        bool help = false;
        bool verbose = false;
        bool invert_match = false;
        bool ignore_case = false;
        bool exact_match = false;
        bool regex_match = false;
        bool timer = false;
        std::vector<std::string> dbs;
        std::set<std::string> lookup;

        auto cli = clara::Help(help) | clara::Opt(verbose)["-v"]["--verbose"]("Display verbose information") |
                   clara::Opt(timer, "timer")["--timer"]("Display the execution runtime.") |
                   clara::Opt(version)["--version"]("Display the version of fast-locate.") |
                   clara::Opt(ignore_case)["-i"]["--ignore-case"]("Ignore case.") |
                   clara::Opt(invert_match)["-u"]["--invert-match"]("Display lines that do not match given pattern.") |
                   clara::Opt(regex_match)["-r"]["--regex"]("Use regular expression matching algorithm") |
                   clara::Opt(exact_match)["-x"]["--exact-match"]("Use exact match algorithm") |
                   clara::Opt(params.prefix, "prefix")["--prefix"]("Path prefix.") |
                   clara::Opt(dbs, "database")["-d"]["--database"]("The file information database.") |
                   clara::Arg(params.pattern, "pattern")("Search pattern");

        auto result = cli.parse(clara::Args(argc, argv));
        if (!result) {
            fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
            exit(EXIT_FAILURE);
        }

        if (version) {
            disp_version();
            exit(EXIT_SUCCESS);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            fmt::print("{}", oss.str());
            copyright();
            exit(EXIT_SUCCESS);
        }

        if (dbs.empty()) {
            auto default_db = std::getenv("FAST_LOCATE_DB");
            if (default_db == nullptr) {
                params.databases.emplace_back(".database");
            } else {
                params.databases.push_back(default_db);
            }
        } else {
            for (auto item : dbs) {
                lookup.emplace(item);
            }
            for (auto item : lookup) {
                params.databases.push_back(item);
            }
        }

        // If user does not specify prefix then fallback to the
        // FAST-LOCATE_PREFIX environment variable.
        if (params.prefix.empty()) {
            auto const default_prefix = std::getenv("FAST-LOCATE_PREFIX");
            if (default_prefix != nullptr) {
                params.prefix = std::string(default_prefix);
            }
        }

        // Update flags and regex_mode
        exact_match = regex_match ? !regex_match : exact_match;
        params.flags =
            verbose * VERBOSE | invert_match * INVERT_MATCH | exact_match * EXACT_MATCH | ignore_case * IGNORE_CASE;
        params.regex_mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH) | (params.ignore_case() ? HS_FLAG_CASELESS : 0);

        // Display input arguments in JSON format if verbose flag is on
        if (params.verbose()) {
            params.print();
        }

        return params;
    }

    template <typename Matcher, typename Params> void locate(Params &&params) {
        using GrepAlg = ioutils::FileReader<ioutils::LocateStreamPolicy<Matcher>>;
        GrepAlg grep(params);
        for (auto db : params.databases) {
            grep(db.data());
        }
    }
} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    if (params.pattern.empty()) {
        using GrepAlg = ioutils::FileReader<ioutils::PrintAllPolicy>;
        GrepAlg grep(params);
        for (auto db : params.databases) {
            grep(db.data());
        }
    } else {
        if (!params.invert_match()) {
            using Matcher = utils::hyperscan::RegexMatcher;
            locate<Matcher>(params);
        } else {
            using Matcher = utils::hyperscan::RegexMatcherInv;
            locate<Matcher>(params);
        }
    }
    return EXIT_SUCCESS;
}
