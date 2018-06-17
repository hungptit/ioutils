#pragma once

#include "filesystem.hpp"
#include "search.hpp"
#include <set>
#include <string>
#include <ostream>
#include <unordered_set>

namespace ioutils {
    namespace mlocate {
        struct MlocatePolicy {
        public:
            void print() const {
                for (auto const &item : data) {
                    fmt::print("{0:b} {1:>10} {2}\n", item.st_mode & MODE_MASK, item.st_size, item.path);
                }
            }
          protected:
            bool is_valid_dir(const char *dname) const {
                return filesystem::is_valid_dir(dname);
            }
            void process_file(std::string &&p) {
                ioutils::Stats info;
                info.path = p;

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

            void process_dir(const std::string) const {}


            std::vector<ioutils::Stats> data;
            struct stat statbuf;
			static constexpr int MODE_MASK = 0xfff;
        };

        // Write search data to database.
        struct MlocateDBWriter {};

        // Read mlocate database
        struct MlocateDBReader {};

    } // namespace mlocate

} // namespace ioutils
