#pragma once

#include "filesystem.hpp"
#include "fmt/format.h"
#include "regex_matchers.hpp"
#include "resources.hpp"
#include <string>

namespace ioutils {
    template <typename Matcher> class RegexStorePolicy {
      public:
        template <typename Params>
        RegexStorePolicy(Params &&params)
            : matcher(params.path_regex, params.regex_mode),

              store_file(!params.ignore_file()),
              store_dir(!params.ignore_dir()),
              store_symlink(!params.ignore_symlink()) {
            buffer.reserve(1023);
        }

        [[nodiscard]] auto get_paths() const -> const std::vector<std::string> & { return paths; }

      protected:
        auto is_valid_dir(const char *dname) const -> bool { return filesystem::is_valid_dir(dname); }

        void process_file(const Path &parent, const char *stem) {
            if (!store_file) {
                return;
            }
            buffer = parent.path + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                paths.push_back(buffer);
            }
        }

        void process_file(const Path &parent) {
            if (!store_file) {
                return;
            }
            if (matcher.is_matched(parent.path.data(), parent.path.size())) {
                paths.push_back(parent.path);
            }
        }

        void process_symlink(const Path &parent, const char *stem) {
            if (!store_symlink) {
                return;
            }
            buffer = parent.path + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                paths.push_back(buffer);
            }
        }

        void process_symlink(const Path &parent) {
            if (!store_symlink) {
                return;
            }
            if (matcher.is_matched(parent.path.data(), parent.path.size())) {
                paths.push_back(parent.path);
            }
        }

        void process_dir(const std::string & /*unused*/) {}

        void process_fifo(const Path & /*unused*/, const char * /*unused*/) {}
        void process_fifo(const Path & /*unused*/) {}

        void process_chr(const Path & /*unused*/, const char * /*unused*/) {}
        void process_chr(const Path & /*unused*/) {}

        void process_blk(const Path & /*unused*/, const char * /*unused*/) {}
        void process_blk(const Path & /*unused*/) {}

        void process_socket(const Path & /*unused*/, const char * /*unused*/) {}
        void process_socket(const Path & /*unused*/) {}

        void process_whiteout(const Path & /*unused*/, const char * /*unused*/) {}
        void process_whiteout(const Path & /*unused*/) {}

        void process_unknown(const Path & /*unused*/, const char * /*unused*/) {}
        void process_unknown(const Path & /*unused*/) {}

        std::string buffer;
        Matcher matcher;
        std::vector<std::string> paths;
        bool store_file = false;
        bool store_dir = false;
        bool store_symlink = false;
    };
} // namespace ioutils
