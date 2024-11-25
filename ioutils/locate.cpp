#include "ioutils/locate.hpp"
#include "fmt/base.h"
#include "fmt/ranges.h"

namespace ioutils {
    void LocateInputArguments::print() const {
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

    void locate_files(const ioutils::LocateInputArguments &params) {
        if (params.pattern.empty()) {
            using GrepAlg = ioutils::StreamReader<PrintAllPolicy>;
            GrepAlg grep(params);
            for (auto db : params.databases) {
                grep(db.data());
            }
        } else {
            if (!params.invert_match()) {
                using Matcher = utils::hyperscan::RegexMatcher;
                find_matched_files<Matcher>(params);
            } else {
                using Matcher = utils::hyperscan::RegexMatcherInv;
                find_matched_files<Matcher>(params);
            }
        }
    }
} // namespace ioutils
