#pragma once

#include <string>
#include <vector>

namespace ioutils {
    struct Parameters {
        bool verbose = false;
        bool invert_match = false;
        bool ignore_case = false;

        int regex_mode = 0;
        int flags = 0;

        std::string regex;
        std::string database;
        std::vector<std::string> paths;
    };
} // namespace ioutils
