#pragma once

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {
    namespace filesystem {
        enum Error : int8_t {
            SUCCESS = 0,
            FAILED = -1,
        };

        auto is_valid_dir(const char *p) -> bool;
        auto is_valid_dir_slow(const char *dname) -> bool;

        auto is_regular_file(mode_t st_mode) -> bool;
        auto is_directory(mode_t st_mode) -> bool;
        auto is_symlink(mode_t st_mode) -> bool;
        auto get_extension(const char *p, size_t len) -> const char *;

        // Check that if a path is exist.
        auto exists(const char *p) -> bool;
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
        template <typename T> explicit Path(int val, T &&p) : fd(val), path(std::forward<T>(p)) {}

        template <typename T> explicit Path(T &&p) : fd(p.fd), path(std::forward<std::string>(p.path)) {}

        int fd; // The current path file descriptor
        std::string path;
    };
} // namespace ioutils
