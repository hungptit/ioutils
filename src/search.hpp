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

#include "filesystem.hpp"
#include "fmt/format.h"

namespace ioutils {
    // A class which has DFS and BFS file traversal algorithms.
    template <typename Policy> class FileSearch : public Policy {
      public:
        // Basic search functionality.
        explicit FileSearch() : folders() {}

        // Filtering files using given patterns.
        explicit FileSearch(const std::string &pattern) : Policy(pattern), folders() {}

        // Filtering files using given extensions.
        explicit FileSearch(const std::vector<std::string> &extensions)
            : Policy(extensions), folders() {}

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
                            Policy::process_file(dir.path + sep + info->d_name);
                            break;
                        case DT_LNK: {
                            // TODO: What should we do with symlink?
                            // 1. Just store the path of symlinks
                            // 2. Push the real paths into stack? We do need to make sure that there is not any duplicated path.
                            std::string path = dir.path + sep + info->d_name;
                            auto len = readlink(path.data(), buffer, buflen);
                            fmt::print("Symlink: {0} -> {1}\n", path, std::string(buffer, len));
                            // throw "How should we handle symlinks";
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
                Policy::process_file(std::move(dir.path));
                ::close(fd);
            } else {
                throw "We should not be here!";
            }
        }

        std::deque<Path> folders;
        static constexpr char sep = '/';
        static constexpr size_t buflen = 1024;
        char buffer[buflen];
    };
} // namespace ioutils
