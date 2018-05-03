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
        // off_t st_size;   /* total size, in bytes */
        // time_t st_atime; /* time of last access */
        // time_t st_mtime; /* time of last modification */
        // time_t st_ctime; /* time of last status change */
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
        int fd;
        std::string path;
    };

    struct FileSearch {
        // template <typename T> DFS(T &&paths) : folders(std::forward<T>(paths)) {}
        const std::vector<Stats> &dfs(const std::vector<Path> &p) {
            // Preprocess input arguments
            for (auto item : p) {
                folders.emplace_back(item);
				fmt::print("{}\n", item.path);
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
            // fmt::print("Enter {}\n", dir.path);
            if (ioutils::filesystem::is_directory(props.st_mode)) {
                DIR *dirp = fdopendir(fd);
                if (dirp != nullptr) {
                    struct dirent *info;
                    while ((info = readdir(dirp)) != NULL) {
                        switch (info->d_type) {
                        case DT_DIR:
                            if ((strcmp(info->d_name, ".") != 0) &&
                                (strcmp(info->d_name, "..") != 0) &&
                                (strcmp(info->d_name, ".git") != 0)) {
                                Path current_dir = dir;
                                current_dir.path.push_back('/');
                                current_dir.path.append(info->d_name);
                                current_dir.fd = ::open(current_dir.path.data(), O_RDONLY);
                                fmt::print("{0}\n", current_dir.path);
                                folders.emplace_back(current_dir);
                            }
                            break;
                        case DT_REG:
                            fmt::print("{0}/{1}\n", dir.path, info->d_name);
                            break;
                        default:
                            break;
                        }
                    }
                    (void)closedir(dirp);
                }
            } else {
                fmt::print("How can we get here?\n");
            }
            ::close(fd);
        }

		void process_file(const Path &dir, const struct dirent* info ) {
			
		}
		
        std::unordered_set<std::string> status;
        std::vector<Stats> files;
        std::vector<Path> folders;
    };

    struct BFS {};

} // namespace ioutils
