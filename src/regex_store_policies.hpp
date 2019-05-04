#pragma once

#include "filesystem.hpp"
#include "fmt/format.h"
#include "resources.hpp"
#include "utils/regex_matchers.hpp"
#include <string>

namespace ioutils {
    template <typename Matcher> class RegexStorePolicy {
      public:
        template <typename Params>
        RegexStorePolicy(Params &&params)
            : buffer(), matcher(params.path_regex, params.regex_mode), paths(),
              store_file(!params.ignore_file()), store_dir(!params.ignore_dir()),
              store_symlink(!params.ignore_symlink()) {
            buffer.reserve(1023);
        }

        const std::vector<std::string> &get_paths() const { return paths; }

      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(const Path &parent, const char *stem) {
            if (!store_file) return;
            buffer = parent.path + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                paths.push_back(buffer);
            }
        }

        void process_file(const Path &parent) {
            if (!store_file) return;
            if (matcher.is_matched(parent.path.data(), parent.path.size())) {
                paths.push_back(parent.path);
            }
        }

        void process_symlink(const Path &parent, const char *stem) {
            if (!store_symlink) return;
            buffer = parent.path + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                paths.push_back(buffer);
            }
        }
        
        void process_symlink(const Path &parent) {
            if (!store_symlink) return;
            if (matcher.is_matched(parent.path.data(), parent.path.size())) {
                paths.push_back(parent.path);
            }
        }

        void process_dir(const std::string &) {}

        void process_fifo(const Path &, const char *) {}
        void process_fifo(const Path &) {}
        
        void process_chr(const Path &, const char *) {}
        void process_chr(const Path &) {}
        
        void process_blk(const Path &, const char *) {}
        void process_blk(const Path &) {}
        
        void process_socket(const Path &, const char *) {}
        void process_socket(const Path &) {}
        
        void process_whiteout(const Path &, const char *) {}
        void process_whiteout(const Path &) {}
		
        void process_unknown(const Path &, const char *) {}
        void process_unknown(const Path &) {}
        
        std::string buffer;
        Matcher matcher;
        std::vector<std::string> paths;
        bool store_file = false;
        bool store_dir = false;
        bool store_symlink = false;
    };
} // namespace ioutils
