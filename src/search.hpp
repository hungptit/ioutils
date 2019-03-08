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
#include <vector>

#include "fdwriter.hpp"
#include "filesystem.hpp"
#include "fmt/format.h"
#include "utils/regex_matchers.hpp"

namespace ioutils {
    // A class which has DFS and BFS file traversal algorithms.
    template <typename Policy> class FileSearch : public Policy {
      public:
        // Basic search functionality.
        template <typename... Args>
        FileSearch(Args... args) : Policy(std::forward<Args>(args)...), current(), next() {}

        // Filtering files using given patterns.
        template <typename T>
        FileSearch(T &&params) : Policy(std::forward<T>(params)), current(), next() {
            current.reserve(512);
            next.reserve(512);
            use_dfs = params.dfs();
            level = params.level;
        }

        template <typename Container> void traverse(Container &&p) {
            if (!use_dfs) {
                bfs(std::forward<Container>(p));
            } else {
                dfs(std::forward<Container>(p));
            }
        }

        template <typename Container> void dfs(Container &&p) {
            for (auto item : p) {
                int fd = ::open(item.data(), O_RDONLY);
                if (fd > -1) next.emplace_back(Path{fd, item});
            }

            // Search for files and folders using DFS traversal.
            while (!next.empty()) {
                auto parent = next.back();
                next.pop_back();
                visit(parent);
            }
        }

        template <typename Container> void bfs(Container &&p) {
            for (auto item : p) {
                int fd = ::open(item.data(), O_RDONLY);
                if (fd > -1) next.emplace_back(Path{fd, item});
            }

            // Search for files and folders using DFS traversal.
            int current_level = 0;
            while (!next.empty()) {
                std::swap(current, next);
                next.clear();
                for (auto current_path : current) {
                    visit(current_path);
                }
                ++current_level;
                if ((level > -1) && (current_level > level)) {
                    break; // Stop if we have traverse to the desired depth.
                }
            }
        }

      private:
        void visit(Path &dir) {
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
                            if (Policy::is_valid_dir(info->d_name)) {
                                std::string p(dir.path + "/" + info->d_name);
                                int current_dir_fd = ::open(p.data(), O_RDONLY);
                                if (current_dir_fd >= 0) {
                                    Policy::process_dir(p);
                                    next.emplace_back(Path{current_dir_fd, std::move(p)});
                                }
                            }
                            break;
                        case DT_REG:
                            Policy::process_file(dir.path, info->d_name);
                            break;
                        case DT_LNK: {
                            Policy::process_symlink(dir.path, info->d_name);
                            break;
                        }
                        default:
                            // We only care about directories, symlinks and regular files.
                            break;
                        }
                    }
                }
                (void)closedir(dirp);
            } else if (ioutils::filesystem::is_regular_file(props.st_mode)) {
                Policy::process_file(dir.path);
                ::close(fd);
            } else {
                throw "We should not be here!";
            }
        }

        std::vector<Path> current;
        std::vector<Path> next;
        bool use_dfs = true;
        int level = -1;
        static constexpr char SEP = '/';
    };
} // namespace ioutils
