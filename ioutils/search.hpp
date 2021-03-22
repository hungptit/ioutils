#pragma once

#include "fmt/format.h"
#include "hs/hs.h"
#include "utilities.hpp"
#include <string>

#include "search_default.hpp"
#include "search_optimistic.hpp"

namespace ioutils {
    namespace search {

        enum FileSearchFlags : uint32_t {
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
            IGNORE_ERROR = 1 << 15,     // Do not print out errors.
        };

        struct SearchInputArguments {
            static constexpr int EXPLORE_ALL = -1;
            int flags = 0;
            int regex_mode = HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH;
            int maxdepth = std::numeric_limits<int>::max();
            std::string path_regex = {};
            std::vector<std::string> paths = {};

            bool verbose() const { return (flags & FileSearchFlags::VERBOSE) > 0; }
            bool invert_match() const { return (flags & FileSearchFlags::INVERT_MATCH) > 0; }
            bool color() const { return (flags & FileSearchFlags::COLOR) > 0; }
            bool ignore_file() const { return (flags & FileSearchFlags::IGNORE_FILE) > 0; }
            bool ignore_dir() const { return (flags & FileSearchFlags::IGNORE_DIR) > 0; }
            bool ignore_symlink() const { return (flags & FileSearchFlags::IGNORE_SYMLINK) > 0; }
            bool ignore_fifo() const { return (flags & FileSearchFlags::IGNORE_FIFO) > 0; }
            bool ignore_chr() const { return (flags & FileSearchFlags::IGNORE_CHR) > 0; }
            bool ignore_blk() const { return (flags & FileSearchFlags::IGNORE_BLK) > 0; }
            bool ignore_socket() const { return (flags & FileSearchFlags::IGNORE_SOCKET) > 0; }
            bool ignore_whiteout() const { return (flags & FileSearchFlags::IGNORE_WHITEOUT) > 0; }
            bool ignore_unknown() const { return (flags & FileSearchFlags::IGNORE_UNKNOWN) > 0; }
            bool donot_ignore_git() const { return (flags & FileSearchFlags::DONOT_IGNORE_GIT) > 0; }
            bool ignore_error() const { return (flags & FileSearchFlags::IGNORE_ERROR) > 0; }
            bool follow_symlink() { return (flags & FileSearchFlags::FOLLOW_SYMLINK) > 0; }
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
                    fmt::print("\033[1;34mmaxdepth: \033[1;32m{}\n", maxdepth);
                    fmt::print("\033[1;34mregex: \033[1;32m\"{}\"\n", path_regex);
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
                    fmt::print("maxdepth: {}\n", maxdepth);
                    fmt::print("regex: \"{}\"\n", path_regex);
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

    } // namespace search
} // namespace ioutils
