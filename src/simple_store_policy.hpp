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

        void process_file(const Path &parent, const char *stem) {
            if (store_file) {
                paths.emplace_back(parent.path + SEP + stem);
            }
        }

        void process_file(const Path &parent) {
            if (store_file) {
                paths.emplace_back(parent.path);
            }
        }

        void process_symlink(const Path &parent, const char *stem) {
            if (store_symlink) {
                paths.emplace_back(parent.path + SEP + stem);
            }
        }
        
        void process_symlink(const Path &parent) {
            if (store_symlink) {
                paths.emplace_back(parent.path);
            }
        }
        
        void process_dir(const std::string &p) {
            if (store_dir) {
                paths.push_back(p);
            }
        }

        void process_fifo(const Path &, const char *){}
        void process_fifo(const Path &){}
        
        void process_chr(const Path &, const char *){}
        void process_chr(const Path &){}
        
        void process_blk(const Path &, const char *){}
        void process_blk(const Path &){}
        
        void process_socket(const Path &, const char *){}
        void process_socket(const Path &){}
        
        void process_whiteout(const Path &, const char *){}
        void process_whiteout(const Path &){}
        
        void process_unknown(const Path &, const char *){}
        void process_unknown(const Path &){}
        
        container_type paths;
    };
} // namespace ioutils
