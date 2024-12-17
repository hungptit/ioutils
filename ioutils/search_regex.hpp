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
#include "fmt/base.h"

namespace ioutils {
    // A class which has DFS and BFS file traversal algorithms.
    template <typename Policy> class FileSearchRegex : public Policy {
      public:
        FileSearchRegex(const std::string &pattern) : Policy(pattern) {}
        void dfs(const std::vector<std::string> &p) {
            for (auto item : p) {
                int fd = ::open(item.data(), O_RDONLY);
                if (fd > -1) {
                    folders.emplace_back(fd, item);
                }
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
                if (fd > -1) {
                    folders.emplace_back(fd, item);
                }
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
            if (retval < 0) {
                return;
            }

            if (ioutils::filesystem::is_directory(props.st_mode)) {
                DIR *dirp = fdopendir(fd);
                if (dirp != nullptr) {
                    struct dirent *info;
                    while ((info = readdir(dirp)) != nullptr) {
                        switch (info->d_type) {
                        case DT_DIR:
                            if (Policy::is_valid_dir(info->d_name)) {
                                std::string p(dir.path + "/" + info->d_name);
                                int current_dir_fd = ::open(p.data(), O_RDONLY);
                                if (current_dir_fd >= 0) {
                                    Policy::process_dir(p);
                                    folders.emplace_back(current_dir_fd, std::move(p));
                                }
                            }
                            break;
                        case DT_REG:
                            Policy::process_file(dir.path + "/" + info->d_name);
                            break;
                        default:
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
    };

    // A policy class which filter files and folder using given regular expression.
    template <typename T> struct RegexConsolePolicy {
        explicit RegexConsolePolicy(const std::string &patt) : matcher(patt) {}

        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

        void process_file(std::string && p) const {
            if (matcher(p.data(), p.size())) fmt::print("{}\n", p);
        }
        void process_dir(const Path &p) const {
            matcher(p.path.data(), p.path.size()) fmt::print("{}\n", p);
        }

        utils::RegexMatcher matcher;
    };
} // namespace ioutils
