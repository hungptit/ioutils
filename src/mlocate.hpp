#pragma once

#include "filesystem.hpp"
#include "search.hpp"
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>

namespace ioutils {
    namespace mlocate {
        struct Policy {
          public:
            using container_type = std::vector<ioutils::Stats>;
            const container_type &get_data() const { return data; }

            void print() const {
                for (auto const &item : data) {
                    fmt::print("{0:b} {1:>10} {2}\n", item.st_mode & MODE_MASK, item.st_size,
                               item.path);
                }
            }

          protected:
            bool is_valid_dir(const char *dname) const {
                return filesystem::is_valid_dir(dname);
            }
            void process_file(const std::string &parent, const char *stem) {
                ioutils::Stats info;
                info.path = parent + "/" + stem;

                // Skip if we cannot get information of a given file
                if (stat(info.path.data(), &statbuf)) return;

                // Update information
                info.st_mode = statbuf.st_mode;
                info.st_size = statbuf.st_size;
                info.last_access_time = statbuf.st_atime;
                info.modification_time = statbuf.st_mtime;
                info.status_change_time = statbuf.st_ctime;

                // Update data
                data.emplace_back(info);
            }

            void process_symlink(const std::string &parent, const char *stem) {
                process_file(parent, stem);
            }

            void process_dir(const std::string) const {}

            container_type data;
            struct stat statbuf;
            static constexpr int MODE_MASK = 0xfff;
        };

    } // namespace mlocate

    template <typename Matcher> class LocatePolicy {
      public:
        template <typename T>
        LocatePolicy(T &&args)
            : matcher(args.pattern, args.parameters.mode), prefix(args.parameters.prefix) {}

        void process(const char *begin, const size_t len) {
            constexpr char EOL = '\n';
            const char *start = begin;
            const char *end = begin + len;
            const char *ptr = begin;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                process_line(start, ptr - start + 1);
                start = ++ptr;
                if (start == end) break;
            }
        }

      private:
        Matcher matcher;
        std::string prefix;
        void process_line(const char *begin, const size_t len) {
            if (matcher.is_matched(begin, len)) {
                fmt::print("{0}{1}", prefix, std::string(begin, len));
            }
        }
    };

    class PrintAllPolicy {
      public:
        template <typename Params> PrintAllPolicy(Params &&args) : prefix(args.parameters.prefix) {}
        void process(const char *begin, const size_t len) {
            constexpr char EOL = '\n';
            const char *start = begin;
            const char *end = begin + len;
            const char *ptr = begin;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                process_line(start, ptr - start + 1);
                start = ++ptr;
                if (start == end) break;
            }
        }

      private:
        std::string prefix;
        void process_line(const char *begin, const size_t len) {
            fmt::print("{0}{1}", prefix, std::string(begin, len));
        }
    };
} // namespace ioutils
