#pragma once

#include "filesystem.hpp"
#include "resources.hpp"
#include "fmt/format.h"

namespace ioutils {
    class AllPassFilter {
      protected:
        bool is_valid_file(const std::string &) { return true; }
    };

    // Filter files using given extensions.
    class ExtensionFilter {
        explicit ExtensionFilter(const std::vector<std::string> &exts) : extensions(exts) {}

      protected:
        bool is_valid_file(const std::string &) { return true; }
        std::vector<std::string> extensions;
    };

    // Filter file using group
    class GroupFilter {
        explicit GroupFilter(const int id) : user_id(id) {}

      protected:
        bool is_valid_file(const std::string &) { return true; }
        int user_id;
    };

    // A policy class that display folder and file paths to console.
    class ConsolePolicy {
      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(const Path &parent, const char *stem) const {
            fmt::print("{0}/{1}\n", parent.path, stem);
        }

        void process_file(const Path &parent) const { fmt::print("{0}/{1}\n", parent.path); }

        void process_symlink(const Path &parent, const char *stem) const {
            fmt::print("{0}/{1}\n", parent.path, stem);
        }

        void process_dir(const std::string &p) const { fmt::print("{}\n", p); }
    };

} // namespace ioutils
