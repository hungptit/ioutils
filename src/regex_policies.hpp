#pragma once

#include "filesystem.hpp"
#include "fmt/format.h"
#include "utils/regex_matchers.hpp"
#include <string>

namespace ioutils {
    template <typename Matcher> class RegexPolicy {
      public:
        RegexPolicy(const std::string &pattern,
                    const int mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH))
            : buffer(), matcher(pattern, mode) {
            buffer.reserve(1023);
        }

      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
        void process_file(const std::string &parent, const char *stem) {
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                fmt::print("{}\n", buffer);
            }
        }

        void process_dir(const std::string &p) {
            if (matcher.is_matched(p.data(), p.size())) {
                fmt::print("{}\n", p);
            }
        }

        std::string buffer;
        Matcher matcher;
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
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                files.push_back(buffer);
            }
        }

        void process_dir(const std::string &p) {}

        std::string buffer;
        Matcher matcher;
        std::vector<std::string> files;
    };
} // namespace ioutils
