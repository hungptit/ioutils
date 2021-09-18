#include "fmt/format.h"
#include "ioutils/search.hpp"

namespace ioutils {
    void SearchInputArguments::print() const {
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
} // namespace ioutils
