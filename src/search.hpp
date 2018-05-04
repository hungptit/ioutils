#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <set>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_set>

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
        enum Error : int8_t {
            SUCCESS = 0,
            FAILED = -1,
        };

        bool is_regular_file(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFREG; }

        bool is_directory(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFDIR; }

        bool is_symlink(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFLNK; }

        struct Utils {
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

            char fullpath[PATH_MAX];
        };

        struct Path {
            int fd;
            char fullpath[1024];
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

    // A struct which holds required directory information.
    struct DirStat {
        // DirStat(const std::string &p) : path(p) {}
        // DirStat(std::string &&p) : path(std::move(p)) {}
        // template <typename T> DirStat(T &&p) : path(std::forward<T>(p.path)) {}
        std::string path;
    };

    struct Path {
        template <typename T> explicit Path(int val, T &&p) : fd(val), path(p) {}
        int fd;
        std::string path;
    };

    struct FileSearch {
        // template <typename T> DFS(T &&paths) : folders(std::forward<T>(paths)) {}
        const std::vector<Stats> &dfs(const std::vector<Path> &p) {
            // Preprocess input arguments
            for (auto item : p) {
                folders.emplace_back(item);
                // fmt::print("{}\n", item.path);
            }

            // Search for files and folders using DFS traversal.
            while (!folders.empty()) {
                auto parent = folders.back();
                folders.pop_back();
                visit(parent);
            }
            return files;
        }

        void visit(Path &dir) {
            struct stat props;
            const int fd = dir.fd;
            int retval = fstat(fd, &props);
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
								}
                            }
                            break;
                        case DT_REG:
                            fmt::print("{0}{1}{2}\n", dir.path, "/", info->d_name);
                            break;
                        default:
                            break;
                        }
                    }
                }
                (void)closedir(dirp);
            } else if (ioutils::filesystem::is_regular_file(props.st_mode)) {
                fmt::print("{}\n", dir.path);
            } else {
                fmt::print("How can we get here?\n");
            }

            ::close(fd);
        }

        void process_file(std::string &&path, struct stat *props) const {
            fmt::print("{0}/{1}\n", path);
        }

        void process_dir(const Path &dir, const struct dirent *info) {
            Path current_dir(,
                             dir.path + "/" + info->d_name);
            fmt::print("{0}\n", current_dir.path);
            folders.emplace_back(current_dir);
        }

        bool is_valid_dir(const char *dname) const {
            return (strcmp(dname, ".") != 0) && (strcmp(dname, "..") != 0) &&
                   (strcmp(dname, ".git") != 0);
        }

        std::vector<Path> folders;
        std::vector<Stats> files;
    };
} // namespace ioutils
