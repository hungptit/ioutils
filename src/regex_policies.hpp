#pragma once

#include "filesystem.hpp"
#include "fmt/format.h"
#include "resources.hpp"
#include "utils/regex_matchers.hpp"
#include <string>

namespace ioutils {
    template <typename Matcher, typename Params> class RegexPolicy {
      public:
        RegexPolicy(const std::string &pattern, Params &&params)
            : buffer(), matcher(pattern, params.regex_mode),
              parameters(std::forward<Params>(params)) {
            buffer.reserve(1023);
            display_file = (params.type & DisplayType::DISP_FILE) > 0;
            display_dir = (params.type & DisplayType::DISP_DIR) > 0;
            display_symlink = (params.type & DisplayType::DISP_SYMLINK) > 0;
            color = (params.type & DisplayType::DISP_COLOR) > 0;
        }

      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(const std::string &parent, const char *stem) {
            if (!display_file) return;
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                if (!color) {
                    fmt::print("{}\n", buffer);
                } else {
                    fmt::print("\033[1;39m{}\033[0m\n", buffer);
                }
            }
        }

        void process_symlink(const std::string &parent, const char *stem) {
            if (!display_symlink) return;
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                if (!color) {
                    fmt::print("{}\n", buffer);
                } else {
                    fmt::print("\033[1;31m{}\033[0m\n", buffer);
                }
            }
        }

        void process_dir(const std::string &p) {
            if (!display_dir) return;
            if (matcher.is_matched(p.data(), p.size())) {
                if (!color) {
                    fmt::print("{}\n", p);
                } else {
                    fmt::print("\033[1;32m{}\033[0m\n", p);
                }
            }
        }

        std::string buffer;
        Matcher matcher;
        Params parameters;

        // Display functionality related flags.
        bool display_file;
        bool display_dir;
        bool display_symlink;
        bool color;
    };

    template <typename Matcher> class RegexStorePolicy {
      public:
        RegexStorePolicy(const std::string &pattern,
                         const int mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH))
            : buffer(), matcher(pattern, mode) {
            buffer.reserve(1023);
        }

        const std::vector<std::string> &get_files() const { return files; }

      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(const std::string &parent, const char *stem) {
            if (!store_file) return;
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                files.push_back(buffer);
            }
        }

        void process_symlink(const std::string &parent, const char *stem) {
            if (!store_symlink) return;
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                files.push_back(buffer);
            }
        }

        void process_dir(const std::string &p) {}

        std::string buffer;
        Matcher matcher;
        std::vector<std::string> files;

        static constexpr bool store_file = true;
        static constexpr bool store_symlink = true;
    };
} // namespace ioutils
