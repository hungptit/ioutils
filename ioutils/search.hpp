#pragma once

#include "utilities.hpp"
#include <limits>
#include <string>

#include "ioutils/enums.hpp"
#include "ioutils/search_default.hpp"

namespace ioutils {
    auto get_default_regex_mode() -> int;

    struct SearchInputArguments {
        static constexpr int EXPLORE_ALL = -1;
        int flags = 0;
        int regex_mode = get_default_regex_mode();
        int maxdepth = std::numeric_limits<int>::max();
        std::string path_regex = {};
        std::vector<std::string> paths = {};

        [[nodiscard]] auto verbose() const -> bool { return (flags & FileSearchFlags::VERBOSE) > 0; }
        [[nodiscard]] auto invert_match() const -> bool {
            return (flags & FileSearchFlags::INVERT_MATCH) > 0;
        }
        [[nodiscard]] auto color() const -> bool { return (flags & FileSearchFlags::COLOR) > 0; }
        [[nodiscard]] auto ignore_file() const -> bool {
            return (flags & FileSearchFlags::IGNORE_FILE) > 0;
        }
        [[nodiscard]] auto ignore_dir() const -> bool { return (flags & FileSearchFlags::IGNORE_DIR) > 0; }
        [[nodiscard]] auto ignore_symlink() const -> bool {
            return (flags & FileSearchFlags::IGNORE_SYMLINK) > 0;
        }
        [[nodiscard]] auto ignore_fifo() const -> bool {
            return (flags & FileSearchFlags::IGNORE_FIFO) > 0;
        }
        [[nodiscard]] auto ignore_chr() const -> bool { return (flags & FileSearchFlags::IGNORE_CHR) > 0; }
        [[nodiscard]] auto ignore_blk() const -> bool { return (flags & FileSearchFlags::IGNORE_BLK) > 0; }
        [[nodiscard]] auto ignore_socket() const -> bool {
            return (flags & FileSearchFlags::IGNORE_SOCKET) > 0;
        }
        [[nodiscard]] auto ignore_whiteout() const -> bool {
            return (flags & FileSearchFlags::IGNORE_WHITEOUT) > 0;
        }
        [[nodiscard]] auto ignore_unknown() const -> bool {
            return (flags & FileSearchFlags::IGNORE_UNKNOWN) > 0;
        }
        [[nodiscard]] auto donot_ignore_git() const -> bool {
            return (flags & FileSearchFlags::DONOT_IGNORE_GIT) > 0;
        }
        [[nodiscard]] auto ignore_error() const -> bool {
            return (flags & FileSearchFlags::IGNORE_ERROR) > 0;
        }
        [[nodiscard]] auto follow_symlink() const -> bool {
            return (flags & FileSearchFlags::FOLLOW_SYMLINK) > 0;
        }
        [[nodiscard]] auto dfs() const -> bool { return (flags & DFS) > 0; }

        void print() const;
        void set_default_regex_mode();
    };

} // namespace ioutils
