#pragma once

#include "utilities.hpp"
#include <limits>
#include <string>

#include "ioutils/enums.hpp"
#include "ioutils/search_default.hpp"

namespace ioutils {
    int get_default_regex_mode();
    
    struct SearchInputArguments {
        static constexpr int EXPLORE_ALL = -1;
        int flags = 0;
        int regex_mode = get_default_regex_mode();
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
        bool follow_symlink() const { return (flags & FileSearchFlags::FOLLOW_SYMLINK) > 0; }
        bool dfs() const { return (flags & DFS) > 0; }

        void print() const;
        void set_default_regex_mode();
    };

} // namespace ioutils
