#include "fmt/format.h"
#include <array>
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

namespace ioutils {
    namespace filesystem {
        namespace {
            constexpr int NumberOfStems = 1;
            const std::array<std::string, NumberOfStems + 1> lookup_table{{".", ".."}};

            template <int N> bool is_valid_dir(const char *p) {
                return (strcmp(p, lookup_table[N].data()) != 0) && is_valid_dir<N - 1>(p);
            }

            template <> bool is_valid_dir<0>(const char *p) { return strcmp(p, lookup_table[0].data()) != 0; }
        } // namespace

        bool is_valid_dir(const char *p) { return is_valid_dir<NumberOfStems>(p); }

        bool is_valid_dir_slow(const char *dname) {
            return (strcmp(dname, ".") != 0) && (strcmp(dname, "..") != 0) && (strcmp(dname, ".git") != 0);
        }

        enum Error : int8_t {
            SUCCESS = 0,
            FAILED = -1,
        };

        bool is_regular_file(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFREG; }

        bool is_directory(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFDIR; }

        bool is_symlink(const mode_t st_mode) { return (st_mode & S_IFMT) == S_IFLNK; }

        const char *get_extension(const char *p, const size_t len) {
            const char *pos = p + len - 1;
            while (pos != p) {
                if (*pos == '.') return pos;
                --pos;
            }
            return nullptr;
        }

        // Check that if a path is exist.
        bool exists(const char *p) {
            struct stat buf;
            return stat(p, &buf) == 0;
        }
    } // namespace filesystem
} // namespace ioutils
