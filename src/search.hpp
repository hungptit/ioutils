#pragma once

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
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

// cereal
#include "cereal/types/array.hpp"
#include "cereal/types/chrono.hpp"
#include "cereal/types/deque.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

namespace ioutils {
    // A class which has DFS and BFS file traversal algorithms.
    template <typename Policy> class FileSearch : public Policy {
      public:
        // Basic search functionality.
        template <typename... Args>
        FileSearch(Args... args) : Policy(std::forward<Args>(args)...), folders() {}

        // Filtering files using given patterns.
        template <typename T>
        FileSearch(T &&params) : Policy(std::forward<T>(params)), folders() {}

        template <typename Container> void dfs(Container &&p) {
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

        template <typename Container> void bfs(Container &&p) {
            for (auto item : p) {
                int fd = ::open(item.data(), O_RDONLY);
                if (fd > -1) folders.emplace_back(Path{fd, item});
            }

            // Search for files and folders using DFS traversal.
            while (!folders.empty()) {
                auto parent = folders.front();
                folders.pop_front();
                visit(parent);
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
                                    folders.emplace_back(Path{current_dir_fd, std::move(p)});
                                }
                            }
                            break;
                        case DT_REG:
                            Policy::process_file(dir.path, info->d_name);
                            break;
                        case DT_LNK: {
                            // We only store symlink path.
                            Policy::process_symlink(dir.path, info->d_name);
                            break;
                        }

                        default:
                            // We only care about directories and regular files.
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

        std::deque<Path> folders;
        static constexpr char SEP = '/';
    };
} // namespace ioutils
