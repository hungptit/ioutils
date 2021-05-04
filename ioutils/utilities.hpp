#pragma once
#include <string>

namespace ioutils {
    void copyright();

    namespace path {
        /**
         * This function will simplify the given path string so that the displayed path
         * makes sense to users.
         *
         * Runtime: O(n)
         * Memory: O(n)
         */
        std::string simplify_path(const std::string &path);

    } // namespace path

    namespace search {
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
        auto decompose(const char *begin, const char *end);

    } // namespace search
} // namespace ioutils
