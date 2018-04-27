#pragma once

#include <algorithm>
#include <set>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_set>

// cereal
#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/chrono.hpp"
#include "cereal/types/deque.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

namespace ioutils {
    using DefaultIArchive = cereal::BinaryInputArchive;
    using DefaultOArchive = cereal::BinaryOutputArchive;

    // A struct which stores all information about a file.
    struct FileStat {
        mode_t st_mode;  /* protection */
        off_t st_size;   /* total size, in bytes */
        time_t st_atime; /* time of last access */
        time_t st_mtime; /* time of last modification */
        time_t st_ctime; /* time of last status change */
        std::string extension;
        std::string path;
    };

    // A struct which holds required directory information.
    struct DirStat {
        DirStat(const std::string &p) : path(p) {}
        DirStat(std::string &&p) : path(std::move(p)) {}
        template <typename T> DirStat(T &&p) : path(std::forward<T>(p.path)) {}
        std::string path;
    };

    struct DFS {
        template <typename T> DFS(T &&paths) : folders(std::forward<T>(paths)) {}
        const std::vector<FileStat> &search(const std::vector<DirStat> &p) {
            std::copy(p.cbegin(), p.cend(), folder.end());

            while (!folders.empty()) {
                auto aPath = folders.back();
                folders.pop_back();
                visitor.visit(aPath, folders);
            }
        }

        void visit() {}

        std::unordered_set<std::string> status;
        std::vector<FileStat> files;
        std::vector<DirStat> folders;
    };

    struct BFS {};

} // namespace ioutils
