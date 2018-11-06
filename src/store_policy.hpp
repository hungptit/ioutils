#pragma once

#include <string>
#include <vector>
#include "resources.hpp"
#include "filesystem.hpp"

namespace ioutils {
    // A policy class that stores all file paths.
    class StorePolicy {
      public:
        using container_type = std::vector<std::string>;
        const container_type &get_files() const { return files; }

      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(const std::string &parent, const char *stem) {
            files.emplace_back(parent + SEP + stem);
        }

        void process_symlink(const std::string &parent, const char *stem) {
            files.emplace_back(parent + SEP + stem);
        }
        void process_dir(const std::string) const {}
        container_type files;
    };
}
