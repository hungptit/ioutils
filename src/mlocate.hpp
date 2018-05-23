#pragma once

#include "filesystem.hpp"
#include "search.hpp"
#include <string>
#include <unordered_set>
#include <set>

namespace ioutils {
    struct MlocatePolicy {
      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
        void process_file(std::string &&p) {}
        void process_dir(const std::string) const {}

        std::set<std::string> paths;
    };

    // Write search data to database.
    struct MlocateDBWriter {};

    // Read mlocate database
    struct MlocateDBReader {};

} // namespace utils
