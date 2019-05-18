#pragma once
#include <string>

#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>

namespace ioutils {
    namespace path {
        /**
         * This function will simplify the given path string so that the displayed path makes sense to users .
         *
         * Runtime: O(n)
         * Memory: O(n)
         */
        std::string simplify_path(const std::string &path) {
            constexpr char SLASH = '/';
            const int N = static_cast<int>(path.size());
            if (path.empty()) return "";
            std::vector<std::string> tokens;
            int begin = 0;
            int end = N - 1;
            bool relative_path = path[0] != SLASH;

            while (end >= 0 && path[end] == SLASH) --end;
            while (begin < end && (path[begin] == SLASH)) ++begin;

            auto ptr = begin;
            while (begin <= end) {
                while (ptr <= end && path[ptr] != SLASH) ++ptr;
                auto stem = path.substr(begin, ptr - begin);
                if (stem == ".") {
                    // Ignore the current stem
                } else if (stem == "..") {
                    if (tokens.empty()) {
                        if (relative_path) {
                            tokens.push_back("..");
                        }
                    } else {
                        if (tokens.back() == "..") {
                            tokens.push_back("..");
                        } else {
                            tokens.pop_back();
                        }
                    }
                } else {
                    tokens.push_back(stem);
                }
                while (ptr <= end && (path[ptr] == SLASH)) {
                    ++ptr;
                }
                begin = ptr++;
            }

            std::string results;
            if (!relative_path) results.push_back(SLASH);
            if (tokens.empty()) return results;
            results.append(tokens[0]);
            if (tokens.size() > 1) {
                for (size_t idx = 1; idx < tokens.size(); ++idx) {
                    results.push_back(SLASH);
                    results.append(tokens[idx]);
                }
            }
            return results;
        }
    } // namespace path

    namespace search {
        // Find all paths/files that match the given expansion. For
        // example, foo/*.cpp will match all cpp file in the given
        // folders.
        //
        // This class/algorithm should support full regular expression syntax.
        //
        // Algorithm:
        // 1. Will move from left to right using '/' as a marker and will stop if the current
        // path is invalid.

        /**
           Situations that we need to handle:
           1. A given path is valid and do not have any slash.
           2. A given path is valid and do have slash.
           3. A given path is
        **/
        auto decompose(const char *begin, const char *end) {
            constexpr char FWD_SLASH = '/';
            std::string current_path, pattern;
            const char *ptr = begin;
            const char *pos = begin;

            // Search '/' from left to right
            struct stat info;
            while ((ptr = (const char *)memchr(ptr, FWD_SLASH, end - ptr))) {
                current_path.append(pos, ptr - pos);
                int errcode = stat(current_path.data(), &info);
                if (errcode) {
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

    } // namespace search
} // namespace ioutils
