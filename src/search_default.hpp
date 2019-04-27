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
    namespace filesystem {
        // A class which has DFS and BFS file traversal algorithms.
        template <typename Policy> class DefaultSearch : public Policy {
          public:
            // Filtering files using given patterns.
            template <typename T>
            DefaultSearch(T &&params)
                : Policy(std::forward<T>(params)),
                  current(),
                  next(),
                  use_dfs(params.dfs()),
                  follow_link(params.follow_symlink()),
                  donot_ignore_git(params.donot_ignore_git()),
                  ignore_error(params.ignore_error()),
                  level(params.level) {
                current.reserve(512);
                next.reserve(512);
            }

            void traverse(const std::vector<std::string> &paths) {
                for (auto p : paths) {
                    if (!use_dfs) {
                        bfs(p);
                    } else {
                        dfs(p);
                    }
                }
            }

            /**
             * We do not support the maximum exploration depth in bfs because we need to make dfs and bfs
             * compartible. If the exploration level is desired then travese will use bfs algorithm.
             */
            void dfs(const std::string &p) {
                next.emplace_back(Path{-1, p});
                while (!next.empty()) {
                    auto parent = next.back();
                    next.pop_back();
                    visit(parent);
                }
            }

            /**
             * Traverse given paths using bread-first search algorithm.
             */
            void bfs(const std::string &p) {
                int current_level = 0;
                current.emplace_back(Path{-1, p});
                while (!current.empty()) {
                    next.clear();
                    for (auto const &current_path : current) {
                        visit(current_path);
                    }
                    ++current_level;
                    if ((level > -1) && (current_level > level)) {
                        break; // Stop if we have traverse to the desired depth.
                    }
                    std::swap(current, next);
                }
            }

          private:
            void visit(const Path &dir) {
                struct stat props;
                int fd = ::open(dir.path.data(), O_RDONLY);
                if (fd < 0) {
                    if (!ignore_error) fmt::print(stderr, "fast-find: Cannot open '{}': {}.\n", dir.path, strerror(errno));
                    return;
                }

                int retval = fstat(fd, &props);
                if (retval < 0) {
                    if (!ignore_error) fmt::print(stderr, "fast-find: '{}': {}.\n", dir.path, strerror(errno));
                    ::close(fd);
                    return;
                }

                auto const mode = props.st_mode & S_IFMT;
                if (mode == S_IFDIR) { // A directory
                    DIR *dirp = fdopendir(fd);
                    if (dirp != nullptr) {
                        struct dirent *info;
                        while ((info = readdir(dirp)) != NULL) {
                            switch (info->d_type) {
                            case DT_DIR: {
                                // We have to exclude .. and .. folder from our search results.
                                const bool is_valid_dir =
                                    filesystem::is_valid_dir(info->d_name) && Policy::is_valid_dir(info->d_name);
                                if (is_valid_dir) {
                                    std::string p;
                                    if (dir.path != "/") {
                                        p.append(dir.path);
                                    }
                                    p.push_back('/');
                                    p.append(info->d_name);
                                    Policy::process_dir(p);
                                    next.emplace_back(Path{-1, p});
                                }
                                break;
                            }
                            case DT_REG:
                                Policy::process_file(dir, info->d_name);
                                break;
                            case DT_LNK: {
                                Policy::process_symlink(dir, info->d_name);
                                break;
                            }
                            case DT_FIFO: {
                                Policy::process_fifo(dir, info->d_name);
                                break;
                            }
                            case DT_CHR: {
                                Policy::process_chr(dir, info->d_name);
                                break;
                            }
                            case DT_BLK: {
                                Policy::process_blk(dir, info->d_name);
                                break;
                            }
                            case DT_SOCK: {
                                Policy::process_socket(dir, info->d_name);
                                break;
                            }
                            case DT_WHT: {
                                Policy::process_whiteout(dir, info->d_name);
                                break;
                            }
                            case DT_UNKNOWN: {
                                Policy::process_unknown(dir, info->d_name);
                                break;
                            }
                            default:
                                fmt::print(stderr, "Unrecorgnized path: {}\n", dir.path);
                                break;
                            }
                        }
                    }
                    (void)closedir(dirp);
                } else {
                    if (mode == S_IFREG) {
                        Policy::process_file(dir);
                    } else if (mode == S_IFLNK) {
                        Policy::process_symlink(dir);
                    } else if (mode == S_IFIFO) { // Pipe
                        Policy::process_fifo(dir);
                    } else if (mode == S_IFCHR) { // Character special
                        Policy::process_chr(dir);
                    } else if (mode == S_IFBLK) { // Block special
                        Policy::process_blk(dir);
                    } else if (mode == S_IFSOCK) { // Socket special
                        Policy::process_socket(dir);
#ifdef __APPLE__
                    } else if (mode == S_IFWHT) { // Whiteout is not supported in Linux/ext4
                        Policy::process_whiteout(dir);
#endif
                    } else {
                        // https://stackoverflow.com/questions/47078417/readdir-returning-dirent-with-d-type-dt-unknown-for-directories-and
                        Policy::process_unknown(dir);
                        fmt::print(stderr, "Unrecorgnized path: {}\n", dir.path);
                    }
                    ::close(fd);
                }
            }

            std::vector<Path> current;
            std::vector<Path> next;
            bool use_dfs;
            bool follow_link;
            bool donot_ignore_git;
            bool ignore_error;
            int level = -1;
            std::vector<std::string> unvisited_paths;
            static constexpr char SEP = '/';
        };
    } // namespace filesystem
} // namespace ioutils
