#pragma once

#include "clara.hpp"
#include "fmt/format.h"
#include "hs/hs.h"
#include "utilities.hpp"
#include <string>

namespace ioutils {
    namespace search {
        enum PARAMS : uint32_t {
            VERBOSE = 1,                // Display verbose information.
            INVERT_MATCH = 1 << 1,      // Display paths that do not match given pattern.
            COLOR = 1 << 2,             // Display color text.
            IGNORE_FILE = 1 << 3,       // Skip files.
            IGNORE_DIR = 1 << 4,        // Skip directories.
            IGNORE_SYMLINK = 1 << 5,    // Skip symlinks.
            FOLLOW_SYMLINK = 1 << 6,    // Dive into to the symlink.
            DFS = 1 << 7,               // Use DFS for file traversal.
            IGNORE_FIFO = 1 << 8,       // Ignore pipe
            IGNORE_CHR = 1 << 9,        // Ignore character special
            IGNORE_BLK = 1 << 10,       // Ignore block special
            IGNORE_SOCKET = 1 << 11,    // Ignore sockets
            IGNORE_WHITEOUT = 1 << 12,  // Ignore whiteout files
            IGNORE_UNKNOWN = 1 << 13,   // Ignore unknown paths
            DONOT_IGNORE_GIT = 1 << 14, // Ignore git folders.
        };

        struct Params {
            static constexpr int EXPLORE_ALL = -1;
            Params()
                : flags(0),
                  regex_mode(HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH),
                  level(EXPLORE_ALL),
                  regex(),
                  paths() {}

            int flags;
            int regex_mode;
            int level;
            std::string regex;
            std::vector<std::string> paths;

            bool verbose() { return (flags & PARAMS::VERBOSE) > 0; }
            bool invert_match() { return (flags & PARAMS::INVERT_MATCH) > 0; }
            bool color() { return (flags & PARAMS::COLOR) > 0; }

            bool ignore_file() { return (flags & PARAMS::IGNORE_FILE) > 0; }
            bool ignore_dir() { return (flags & PARAMS::IGNORE_DIR) > 0; }
            bool ignore_symlink() { return (flags & PARAMS::IGNORE_SYMLINK) > 0; }
            bool ignore_fifo() { return (flags & PARAMS::IGNORE_FIFO) > 0; }
            bool ignore_chr() { return (flags & PARAMS::IGNORE_CHR) > 0; }
            bool ignore_blk() { return (flags & PARAMS::IGNORE_BLK) > 0; }
            bool ignore_socket() { return (flags & PARAMS::IGNORE_SOCKET) > 0; }
            bool ignore_whiteout() { return (flags & PARAMS::IGNORE_WHITEOUT) > 0; }
            bool ignore_unknown() { return (flags & PARAMS::IGNORE_UNKNOWN) > 0; }

            bool donot_ignore_git() { return (flags & PARAMS::DONOT_IGNORE_GIT) > 0; }

            bool follow_symlink() { return (flags & PARAMS::FOLLOW_SYMLINK) > 0; }

            bool dfs() { return (flags & DFS) > 0; }

            void print() {
                if (color()) {
                    fmt::print("\033[1;34mverbose: \033[1;32m{}\n", verbose());
                    fmt::print("\033[1;34minvert-match: \033[1;32m{}\n", invert_match());
                    fmt::print("\033[1;34mcolor: \033[1;32m{}\n", color());
                    fmt::print("\033[1;34mignore-file: \033[1;32m{}\n", ignore_file());
                    fmt::print("\033[1;34mignore-dir: \033[1;32m{}\n", ignore_dir());
                    fmt::print("\033[1;34mignore-symlink: \033[1;32m{}\n", ignore_symlink());
                    fmt::print("\033[1;34mfollow-symlink: \033[1;32m{}\n", follow_symlink());
                    fmt::print("\033[1;34mdfs: \033[1;32m{}\n", dfs());
                    fmt::print("\033[1;34mlevel: \033[1;32m{}\n", level);
                    fmt::print("\033[1;34mregex: \033[1;32m\"{}\"\n", regex);
                    fmt::print("\033[1;34mpath: \033[1;32m[");
                    if (!paths.empty()) {
                        size_t idx = 0;
                        fmt::print("\"{}\"", paths[idx++]);
                        for (; idx < paths.size(); ++idx) {
                            fmt::print(",\"{}\"", paths[idx++]);
                        }
                    }
                    fmt::print("]\033[0m\n");
                } else {
                    fmt::print("verbose: {}\n", verbose());
                    fmt::print("invert-match: {}\n", invert_match());
                    fmt::print("color: {}\n", color());
                    fmt::print("ignore-file: {}\n", ignore_file());
                    fmt::print("ignore-dir: {}\n", ignore_dir());
                    fmt::print("ignore-symlink: {}\n", ignore_symlink());
                    fmt::print("follow-symlink: {}\n", follow_symlink());
                    fmt::print("dfs: {}\n", dfs());
                    fmt::print("level: {}\n", level);
                    fmt::print("regex: \"{}\"\n", regex);
                    fmt::print("path: [");
                    if (!paths.empty()) {
                        size_t idx = 0;
                        fmt::print("\"{}\"", paths[idx++]);
                        for (; idx < paths.size(); ++idx) {
                            fmt::print(",\"{}\"", paths[idx++]);
                        }
                    }
                    fmt::print("]\n");
                }
            }
        };

        Params parse_input_arguments(int argc, char *argv[]) {
            Params params;
            std::vector<std::string> paths;
            bool help = false;
            bool verbose = false;
            bool ignore_case = false;

            bool ignore_dir = false;
            bool ignore_file = false;
            bool ignore_symlink = false;
            bool ignore_fifo = false;
            bool ignore_chr = false;
            bool ignore_blk = false;
            bool ignore_socket = false;
            bool ignore_whiteout = false;
            bool ignore_unknown = false;
            bool donot_ignore_git = false;

            bool follow_link = false;
            bool color = false;
            bool inverse_match = false;
            bool dfs = true;
            bool bfs = false;

            std::string begin_time, end_time;

            auto cli =
                clara::Help(help) | clara::Opt(verbose)["-v"]["--verbose"]("Display verbose information") |
                clara::Opt(ignore_case)["-i"]["--ignore-case"]("Ignore case") |
                clara::Opt(inverse_match)["--invert-match"](
                    "Display paths that do not match a given path regex.") |
                clara::Opt(ignore_file)["--ignore-file"]("Ignore files.") |
                clara::Opt(ignore_dir)["--ignore-dir"]("Ignore folders.") |
                clara::Opt(ignore_symlink)["--ignore-symlink"]("Ignore symlink.") |

                clara::Opt(ignore_fifo)["--ignore-symlink"]("Ignore named pipe (fifo).") |
                clara::Opt(ignore_chr)["--ignore-chr"]("Ignore character special.") |
                clara::Opt(ignore_blk)["--ignore-blk"]("Ignore block special.") |
                clara::Opt(ignore_socket)["--ignore-socket"]("Ignore sockets.") |
                clara::Opt(ignore_whiteout)["--ignore-whiteout"]("Ignore whiteout files.") |

                clara::Opt(donot_ignore_git)["--donot-ignore-git"]("Don't ignore .git folders.") |

                clara::Opt(color)["-c"]["--color"]("Print out color text.") |
                clara::Opt(dfs)["--dfs"]("Use DFS for traversing.") |
                clara::Opt(bfs)["--bfs"]("Use BFS for traversing. Note that BFS algorithm does not work "
                                         "well for large folders.") |
                clara::Opt(params.regex, "path-regex")["-e"]["--path-regex"]("Search pattern.") |

                // Unsupported options
                clara::Opt(params.level, "level")["--level"]("The search depth.") |
                clara::Opt(follow_link, "follow-link")["--follow-link"]("Follow symbolic links. (WIP)") |
                clara::Opt(begin_time, "newer")["--newer"](
                    "Display paths that are newer than a given timestamp. (WIP)") |
                clara::Opt(end_time, "older")["--older"](
                    "Display paths that are older than a given timestamp. (WIP)") |

                // Required arguments.
                clara::Arg(paths, "paths")("Search paths");

            auto result = cli.parse(clara::Args(argc, argv));
            if (!result) {
                fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
                exit(EXIT_FAILURE);
            }

            // Print out the help document.
            if (help) {
                std::ostringstream oss;
                oss << cli;
                fmt::print("{}\n", "fast-find version 0.1.0");
                fmt::print("{}\n", "Hung Dang <hungptit@gmail.com>");
                fmt::print("{}", oss.str());
                exit(EXIT_SUCCESS);
            }

            // Need to more extensive path extension here using path_regex.
            if (paths.empty()) {
                params.paths.push_back(".");
            } else {
                for (auto p : paths) {
                    remove_trailing_slash(p);
                    params.paths.emplace_back(p);
                }
            }

            // Init the mode for our regular expression engine.
            params.regex_mode = HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH | (ignore_case ? HS_FLAG_CASELESS : 0);

            // Parse the display type
            dfs = dfs ? true : !bfs;
            params.flags =
                ignore_file * PARAMS::IGNORE_FILE | ignore_dir * PARAMS::IGNORE_DIR |
                ignore_symlink * PARAMS::IGNORE_SYMLINK | color * PARAMS::COLOR |
                verbose * PARAMS::VERBOSE | inverse_match * PARAMS::INVERT_MATCH | dfs * PARAMS::DFS |
                ignore_fifo * PARAMS::IGNORE_FIFO | ignore_blk * PARAMS::IGNORE_BLK |
                ignore_chr * PARAMS::IGNORE_CHR | ignore_socket * PARAMS::IGNORE_SOCKET |
                ignore_whiteout * PARAMS::IGNORE_WHITEOUT | ignore_unknown * PARAMS::IGNORE_UNKNOWN |
                donot_ignore_git * PARAMS::DONOT_IGNORE_GIT;

            // Display input arguments in JSON format if verbose flag is on
            if (params.verbose()) {
                params.print();
            }

            return params;
        }
    } // namespace search
} // namespace ioutils
