#include <string>

#include "fmt/base.h"
#include "ioutils/utilities.hpp"
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>
#include <vector>

namespace {}

namespace ioutils {
    void copyright() { fmt::print("\n{}\n", "Report bugs or enhancement requests to hungptit@gmail.com"); }
} // namespace ioutils

namespace ioutils::path {
    /**
     * Create a compact representation of a given path.
     *
     * Runtime: O(n)
     * Memory: O(n)
     */
    auto simplify_path(const std::string &path) -> std::string {
        constexpr char DOT_DOT[] = "..";
        constexpr char DOT[] = ".";
        constexpr char SLASH = '/';
        constexpr size_t MAX_PATH_COMPONENTS = 32;

        const int N = static_cast<int>(path.size());
        if (path.empty()) {
            return "";
        }

        // Pre-allocate result string to avoid reallocations
        std::string results;
        results.reserve(N);

        // Skip trailing slashes
        int end = N - 1;
        while (end >= 0 && path[end] == SLASH) {
            --end;
        }

        // Skip leading slashes and determine if path is relative
        int begin = 0;
        bool relative_path = path[0] != SLASH;
        while (begin < end && path[begin] == SLASH) {
            ++begin;
        }

        // Handle root path case
        if (begin > end) {
            return relative_path ? DOT : "/";
        }

        // Pre-allocate tokens vector with a reasonable size
        std::vector<std::string_view> tokens;
        tokens.reserve(MAX_PATH_COMPONENTS);

        // Process path components
        auto ptr = begin;
        while (begin <= end) {
            // Find next slash
            while (ptr <= end && path[ptr] != SLASH) {
                ++ptr;
            }

            // Extract token
            const std::string_view stem(path.data() + begin, ptr - begin);

            if (stem.empty()) {
                // Skip empty components
            } else if (stem == DOT) {
                // Ignore current directory
            } else if (stem == DOT_DOT) {
                if (tokens.empty()) {
                    if (relative_path) {
                        tokens.push_back(DOT_DOT);
                    }
                } else {
                    if (tokens.back() == DOT_DOT) {
                        tokens.push_back(DOT_DOT);
                    } else {
                        tokens.pop_back();
                    }
                }
            } else {
                tokens.push_back(stem);
            }

            // Skip consecutive slashes
            while (ptr <= end && path[ptr] == SLASH) {
                ++ptr;
            }
            begin = ptr++;
        }

        // Handle empty tokens case
        if (tokens.empty()) {
            return relative_path ? DOT : "/";
        }

        // Calculate final string size to avoid reallocations
        size_t final_size = tokens.size() - 1; // For slashes
        for (const auto &token : tokens) {
            final_size += token.size();
        }
        if (!relative_path) {
            final_size += 1; // For leading slash
        }
        results.reserve(final_size);

        // Build result string efficiently
        if (!relative_path) {
            results.push_back(SLASH);
        }

        // Append first token
        results.append(tokens[0]);

        // Append remaining tokens with slashes
        for (size_t idx = 1; idx < tokens.size(); ++idx) {
            results.push_back(SLASH);
            results.append(tokens[idx]);
        }

        return results;
    }
} // namespace ioutils::path

namespace ioutils::search {
    // Find all paths/files that match the given expansion. For
    // example, foo/*.cpp will match all cpp file in the given
    // folders.
    //
    // This class/algorithm should support full regular expression syntax.
    //
    // Algorithm:
    // 1. Will move from left to right using '/' as a marker and will stop if the
    // current path is invalid.

    /**
       Situations that we need to handle:
       1. A given path is valid and do not have any slash.
       2. A given path is valid and do have slash.
       3. A given path is
    **/
    auto decompose(const char *begin, const char *end) {
        constexpr char FWD_SLASH = '/';
        std::string current_path;
        std::string pattern;
        const char *ptr = begin;
        const char *pos = begin;

        // Search '/' from left to right
        struct stat info;
        while ((ptr = (const char *)memchr(ptr, FWD_SLASH, end - ptr)) != nullptr) {
            current_path.append(pos, ptr - pos);
            int errcode = stat(current_path.data(), &info);
            if (errcode != 0) {
                current_path.resize(pos - begin);
                break; // Exit the loop if the current path is invalid.
            }
            pos = ++ptr;
        }

        if (current_path.empty()) {
            current_path.append(".");
        }

        if (end > pos) {
            pattern.append(pos, end - pos);
        }

        return std::tie(current_path, pattern);
    }
} // namespace ioutils::search
