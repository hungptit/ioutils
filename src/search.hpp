#pragma once

#include <algorithm>
#include <set>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_set>
#include <fcntl.h>
#include <ctime>
#include <dirent.h>
#include <stdlib.h>

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

	namespace filesystem {
		enum Error : int8_t {
			SUCCESS = 0,
			FAILED = -1,
		};
		
		bool is_regular_file(const mode_t st_mode) {
            return (st_mode & S_IFMT) == S_IFREG;
        }

        bool is_directory(const mode_t st_mode) {
            return (st_mode & S_IFMT) == S_IFDIR;
        }

        bool is_symlink(const mode_t st_mode) {
            return (st_mode & S_IFMT) == S_IFLNK;
        }

		struct Utils {
			const char *get_absolute_path(const char *p, Error &errcode) {
				const char *results = realpath(p, fullpath);
				errcode = (results != nullptr) ? SUCCESS : FAILED;
				return results;
			}

			const char* get_current_directory(Error &errcode) {
				const char* p = getcwd(fullpath, PATH_MAX);
				errcode = (p != nullptr) ? SUCCESS : FAILED;
				return p;
			}
			
			char fullpath[PATH_MAX]; 
		};

		struct Path {
			int fd;
			char fullpath[1024] ;
		};
	}
	
    // A struct which stores all information about a file.
    struct FileStat {
        mode_t st_mode;  /* protection */
        // off_t st_size;   /* total size, in bytes */
        // time_t st_atime; /* time of last access */
        // time_t st_mtime; /* time of last modification */
        // time_t st_ctime; /* time of last status change */
        std::string extension;
        std::string path;
    };

    // A struct which holds required directory information.
    struct DirStat {
        // DirStat(const std::string &p) : path(p) {}
        // DirStat(std::string &&p) : path(std::move(p)) {}
        // template <typename T> DirStat(T &&p) : path(std::forward<T>(p.path)) {}
        std::string path;
    };

    struct DFS {
        template <typename T> DFS(T &&paths) : folders(std::forward<T>(paths)) {}
        const std::vector<FileStat> &search(const std::vector<DirStat> &p) {
            std::copy(p.cbegin(), p.cend(), folders.end());

            while (!folders.empty()) {
                auto parent = folders.back();
                folders.pop_back();
                visit(parent);
            }
            return files;
        }

        void visit(DirStat &dir) {}

        std::unordered_set<std::string> status;
        std::vector<FileStat> files;
        std::vector<DirStat> folders;
    };

    struct BFS {};

} // namespace ioutils
