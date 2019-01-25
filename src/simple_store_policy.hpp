#pragma once

#include "resources.hpp"
#include <string>
#include <vector>

namespace ioutils {
    // A policy class that stores all file paths.
    class StorePolicy {
      public:
        using container_type = std::vector<std::string>;

        template <typename Params>
        StorePolicy(Params &&params)
            : store_file(!params.ignore_file()), store_dir(!params.ignore_dir()),
              store_symlink(!params.ignore_symlink()) {}
        const container_type &get_paths() const { return paths; }

      protected:
        bool store_file = false;
        bool store_dir = false;
        bool store_symlink = false;

        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(const std::string &parent, const char *stem) {
            if (store_file) {
                paths.emplace_back(parent + SEP + stem);
            }
        }

        void process_file(const std::string &parent) {
            if (store_file) {
                paths.emplace_back(parent);
            }
        }

        void process_symlink(const std::string &parent, const char *stem) {
            if (store_symlink) {
                paths.emplace_back(parent + SEP + stem);
            }
        }
        void process_dir(const std::string &p) {
            if (store_dir) {
                paths.push_back(p);
            }
        }
        container_type paths;
    };
} // namespace ioutils
