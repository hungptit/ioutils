#pragma once

#include <string>
#include <vector>

namespace ioutils {
    struct Parameters {
        bool verbose = false;
        bool invert_match = false;
        bool ignore_case = false;
        std::vector<std::string> paths;
        std::string database;
    };
}
