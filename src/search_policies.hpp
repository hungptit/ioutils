#pragma once
#include "filesystem.hpp"

namespace ioutils {
    constexpr char SEP = '/';
    class AllPassFilter {
      protected:
        bool is_valid_file(const std::string &) { return true; }
    };

    // Filter files using given extensions.
    class ExtensionFilter {
        explicit ExtensionFilter(const std::vector<std::string> &exts) : extensions(exts) {}

      protected:
        bool is_valid_file(const std::string &path) { return true; }
        std::vector<std::string> extensions;
    };

    // Filter file using group
    class GroupFilter {
        explicit GroupFilter(const int id) : user_id(id) {}

      protected:
        bool is_valid_file(const std::string &fname) { return true; }
        int user_id;
    };

    // A policy class that display folder and file paths to console.
    class ConsolePolicy {
      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
        void process_file(const std::string &parent, const char *stem) const {
            fmt::print("{0}/{1}\n", parent, stem);
        }
        void process_dir(const std::string &p) const { fmt::print("{}\n", p); }
    };

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
        void process_dir(const std::string) const {}
        container_type files;
    };

    // Filter files using given pattern.
    // TODO: Need to think carefully about the use cases.
    struct RegexFilter {
        explicit RegexFilter(const std::string &patt) : pattern(patt){};
        bool is_valid_file(const std::string &fname) { return true; }

      protected:
        std::string pattern;
    };
} // namespace ioutils