#pragma once

#include "filesystem.hpp"
#include "fmt/format.h"
#include "utils/regex_matchers.hpp"
#include <string>

namespace ioutils {
    enum DisplayType : uint8_t {
        DISP_NONE = 0,
        DISP_FILE = 1,
        DISP_DIR = 2,
        DISP_SYMLINK = 4,
        DISP_EXECUTABLE = 8
    };

    template <typename Matcher, typename Params> class RegexPolicy {
      public:
        RegexPolicy(const std::string &pattern, const Params &params)
            : buffer(), matcher(pattern, params.regex_mode), parameters(params) {
            buffer.reserve(1023);
            display_file = params.type & DisplayType::DISP_FILE;
            display_dir = params.type & DisplayType::DISP_DIR;
            display_symlink = params.type & DisplayType::DISP_SYMLINK;
        }

      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(const std::string &parent, const char *stem) {
            if (!display_file) return;
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                fmt::print("{}\n", buffer);
            }
        }

        void process_symlink(const std::string &parent, const char *stem) {
            if (!display_symlink) return;
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                fmt::print("{}\n", buffer);
            }
        }

        void process_dir(const std::string &p) {
            if (!display_dir) return;
            if (matcher.is_matched(p.data(), p.size())) {
                fmt::print("{}\n", p);
            }
        }

        std::string buffer;
        Matcher matcher;
        Params parameters;

        // Display functionality related flags.
        bool display_file;
        bool display_dir;
        bool display_symlink;
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
            if (!display_file) return;
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                files.push_back(buffer);
            }
        }

        void process_symlink(const std::string &parent, const char *stem) {
            if (!display_symlink) return;
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                files.push_back(buffer);
            }
        }

        void process_dir(const std::string &p) {}

        std::string buffer;
        Matcher matcher;
        std::vector<std::string> files;

        // Display functionality related flags.
        bool display_file;
        bool display_dir;
        bool display_symlink;
    };
} // namespace ioutils
