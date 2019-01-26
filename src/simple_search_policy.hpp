#pragma once

#include "filesystem.hpp"
#include "fmt/format.h"
#include <string>

namespace ioutils {
    class SimplePolicy {
      public:
        template <typename Params>
        SimplePolicy(Params &&params)
            : display_dir(!params.ignore_dir()), display_file(!params.ignore_file()),
              display_symlink(!params.ignore_symlink()), color(params.color()) {}

      protected:
        bool display_dir;
        bool display_file;
        bool display_symlink;
        bool color;

        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(const std::string &parent, const char *stem) const {
            if (display_file) {
                if (!color) {
                    fmt::print("{0}/{1}\n", parent, stem);
                } else {
                    fmt::print("\033[1;39m{0}/{1}\033[0m\n", parent, stem);
                }
            }
        }

        void process_file(const std::string &parent) const {
            if (display_file) {
                if (!color) {
                    fmt::print("{0}\n", parent);
                } else {
                    fmt::print("\033[1;39m{0}\033[0m\n", parent);
                }
            }
        }

        void process_symlink(const std::string &parent, const char *stem) const {
            if (display_symlink) {
                if (!color) {
                    fmt::print("{0}/{1}\n", parent, stem);
                } else {
                    fmt::print("\033[1;31m{0}/{1}\033[0m\n", parent, stem);
                }
            }
        }

        void process_dir(const std::string &p) const {
            if (display_dir) {
                if (!color) {
                    fmt::print("{}\n", p);
                } else {
                    fmt::print("\033[1;32m{}\033[0m\n", p);
                }
            }
        }
    };

} // namespace ioutils
