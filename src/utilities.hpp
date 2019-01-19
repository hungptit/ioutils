#pragma once
#include <string>

#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>

namespace ioutils {
    void remove_trailing_slash(std::string &s) {
        constexpr char SLASH = '/';
        while (!s.empty()) {
            if (s.back() == SLASH) {
                s.pop_back();
            } else {
                break;
            }
        }
    }

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
