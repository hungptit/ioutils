#pragma once

#include "search_params.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {
    namespace search {
        struct PathExpansion {
            void opperator(const std::string &pattern) {
                // Find the root path

                // Search for all files using found root path and patterns
            }

            std::string buffer;
            std::vector<std::string> paths;
        };

        // Find all paths/files that match the given expansion. For
        // example, foo/*.cpp will match all cpp file in the given
        // folders.
        //
        // This class/algorithm should support full regular expression syntax.
        //
        // Algorithm:
        // 1. Will move from left to right using '/' as a marker and will stop if the current
        // path is invalid.
        auto get_root_path(const char *begin, const char *end) {
            Params params;
            constexpr char FWD_SLASH = '/';
            std::string root_path, pattern;
            std::string current_path;
            const char *ptr = begin;
            const char *pos = begin;
            std::vector<int> fds;

            // Search '/' from left to right
            struct stat info;
            while ((ptr = (const char*)memchr(ptr, FWD_SLASH, end - ptr))) {
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

            // Update pattern
            if (pos < end) {
                params.path_regex.append(pos, end - pos);
            }

            // Update paths.
            params.paths.emplace_back(current_path);

            return params;
        }

    } // namespace search
} // namespace ioutils
