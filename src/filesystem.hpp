#pragma once

#include "fmt/format.h"
#include <array>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace ioutils {
    namespace filesystem {
        namespace {
            constexpr int NumberOfStems = 1;
            const std::array<std::string, NumberOfStems + 1> lookup_table{{".", ".."}};

            template <int N> bool is_valid_dir(const char *p) {
                return (strcmp(p, lookup_table[N].data()) != 0) && is_valid_dir<N - 1>(p);
            }

            template <> bool is_valid_dir<0>(const char *p) {
                return strcmp(p, lookup_table[0].data()) != 0;
            }
        } // namespace

        bool is_valid_dir(const char *p) { return is_valid_dir<NumberOfStems>(p); }

        bool is_valid_dir_slow(const char *dname) {
            return (strcmp(dname, ".") != 0) && (strcmp(dname, "..") != 0) &&
                   (strcmp(dname, ".git") != 0);
        }

        enum Error : int8_t {
            SUCCESS = 0,
            FAILED = -1,
        };

        bool is_regular_file(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFREG; }

        bool is_directory(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFDIR; }

        bool is_symlink(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFLNK; }

        const char *get_extension(const char *p, const size_t len) {
            const char *pos = p + len - 1;
            while (pos != p) {
                if (*pos == '.') return pos;
                --pos;
            }
            return nullptr;
        }

        // Check that if a path is exist.
        bool exists(const char *p) {
            struct stat buf;
            return stat(p, &buf) == 0;
        }

        // Utility class for path.
        class Utils {
          public:
            const char *get_absolute_path(const char *p, Error &errcode) {
                const char *results = realpath(p, fullpath);
                errcode = (results != nullptr) ? SUCCESS : FAILED;
                return results;
            }

            const char *get_current_directory(Error &errcode) {
                const char *p = getcwd(fullpath, PATH_MAX);
                errcode = (p != nullptr) ? SUCCESS : FAILED;
                return p;
            }

          private:
            char fullpath[PATH_MAX];
        };
    } // namespace filesystem

    // A struct which stores all information about a file.
    struct Stats {
        mode_t st_mode; /* protection */
        size_t st_size; /* total size, in bytes */
        std::time_t last_access_time;
        std::time_t modification_time;
        std::time_t status_change_time;
        std::string extension;
        std::string path;
    };

    // A struct that hold folder information during the traversal.
    struct Path {
        template <typename T>
        explicit Path(int val, T &&p) : fd(val), path(std::forward<T>(p)) {}

        template <typename T>
        explicit Path(T &&p) : fd(p.fd), path(std::forward<std::string>(p.path)) {}

        int fd; // The current path file descriptor
        std::string path;
    };
} // namespace ioutils
