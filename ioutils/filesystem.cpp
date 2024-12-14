#include <array>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils::filesystem {
    namespace {
        constexpr int NumberOfStems = 1;
        const std::array<std::string, NumberOfStems + 1> lookup_table{{".", ".."}};

        template <int N> auto is_valid_dir(const char *p) -> bool {
            return (strcmp(p, lookup_table[N].data()) != 0) && is_valid_dir<N - 1>(p);
        }

        template <> auto is_valid_dir<0>(const char *p) -> bool {
            return strcmp(p, lookup_table[0].data()) != 0;
        }
    } // namespace

    auto is_valid_dir(const char *p) -> bool { return is_valid_dir<NumberOfStems>(p); }

    auto is_valid_dir_slow(const char *dname) -> bool {
        return (strcmp(dname, ".") != 0) && (strcmp(dname, "..") != 0) && (strcmp(dname, ".git") != 0);
    }

    enum Error : int8_t {
        SUCCESS = 0,
        FAILED = -1,
    };

    auto is_regular_file(const mode_t st_mode) -> bool { return (st_mode & S_IFMT) == S_IFREG; }

    auto is_directory(const mode_t st_mode) -> bool { return (st_mode & S_IFMT) == S_IFDIR; }

    auto is_symlink(const mode_t st_mode) -> bool { return (st_mode & S_IFMT) == S_IFLNK; }

    auto get_extension(const char *p, const size_t len) -> const char * {
        const char *pos = p + len - 1;
        while (pos != p) {
            if (*pos == '.') {
                return pos;
            }
            --pos;
        }
        return nullptr;
    }

    // Check that if a path is exist.
    auto exists(const char *p) -> bool {
        struct stat buf;
        return stat(p, &buf) == 0;
    }
} // namespace ioutils::filesystem
