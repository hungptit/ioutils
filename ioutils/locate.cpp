#include "fmt/format.h"

#include "ioutils/locate.hpp"

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

} // namespace ioutils
