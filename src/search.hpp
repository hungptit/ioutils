#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <dirent.h>
#include <fcntl.h>
#include <set>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_set>
#include <vector>

#include "fmt/format.h"
#include "utils/timeutils.hpp"

// cereal
#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/chrono.hpp"
#include "cereal/types/deque.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

namespace ioutils {
    using DefaultIArchive = cereal::BinaryInputArchive;
    using DefaultOArchive = cereal::BinaryOutputArchive;

    namespace filesystem {
        namespace {
            constexpr int NumberOfStems = 3;
            const std::array<std::string, NumberOfStems> lookup_table{".", "..", ".git"};

            template <int N> bool is_valid_dir(const char *p) {
                return (strcmp(p, lookup_table[N].data()) != 0) && is_valid_dir<N - 1>(p);
            }

            template <> bool is_valid_dir<0>(const char *p) {
                return strcmp(p, lookup_table[0].data()) != 0;
            }
        }

        bool is_valid_dir(const char *p) {
            return is_valid_dir<NumberOfStems>(p);
        }

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

        bool exists(const char *p) {
            struct stat buf;
            return stat(p, &buf) == 0;
        }

        bool is_valid_path(const char *p) {
            int fd = ::open(p, O_RDONLY);
            if (fd > -1) {
                ::close(fd);
                return true;
            }
            return false;
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

    class FileSearch {
      public:
        void dfs(const std::vector<std::string> &p) {
            for (auto item : p) {
                int fd = ::open(item.data(), O_RDONLY);
                if (fd > -1) folders.emplace_back(Path{fd, item});
            }

            // Search for files and folders using DFS traversal.
            while (!folders.empty()) {
                auto parent = folders.back();
                folders.pop_back();
                visit(parent);
            }
        }

        void bfs(const std::vector<std::string> &p) {
            for (auto item : p) {
                int fd = ::open(item.data(), O_RDONLY);
                if (fd > -1) folders.emplace_back(Path{fd, item});
            }

            // Search for files and folders using DFS traversal.
            while (!folders.empty()) {
                auto parent = folders.back();
                folders.pop_back();
                visit(parent);
            }
        }

      protected:
        void visit(const Path &dir) {
            struct stat props;
            const int fd = dir.fd;

            int retval = fstat(fd, &props);
            if (retval < 0) return;

            if (ioutils::filesystem::is_directory(props.st_mode)) {
                DIR *dirp = fdopendir(fd);
                if (dirp != nullptr) {
                    struct dirent *info;
                    while ((info = readdir(dirp)) != NULL) {
                        switch (info->d_type) {
                        case DT_DIR:
                            if (is_valid_dir(info->d_name)) {
                                std::string p(dir.path + "/" + info->d_name);
                                int current_dir_fd = ::open(p.data(), O_RDONLY);
                                if (current_dir_fd >= 0) {
                                    fmt::print("{}\n", p);
                                    folders.emplace_back(Path{current_dir_fd, std::move(p)});
                                }
                            }
                            break;
                        case DT_REG:
                            process_file(dir.path + "/" + info->d_name);
                            break;
                        default:
                            break;
                        }
                    }
                }
                (void)closedir(dirp);
            } else if (ioutils::filesystem::is_regular_file(props.st_mode)) {
                process_file(dir.path);
                ::close(fd);
            } else {
                fmt::print("How can we get here?\n");
            }
        }

        template <typename T> void process_file(T &&p) const { fmt::print("{}\n", p); }

        bool is_valid_dir(const char *dname) const {
            return (strcmp(dname, ".") != 0) && (strcmp(dname, "..") != 0) &&
                   (strcmp(dname, ".git") != 0);
        }

        bool is_valid_file(const char *fname) const { return true; }

        std::deque<Path> folders;
    };
} // namespace ioutils
