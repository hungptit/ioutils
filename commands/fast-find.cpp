#include "clara.hpp"

#include "ioutils/find.hpp"
#include "ioutils/search.hpp"
#include "ioutils/search_params.hpp"
#include "regex_matchers.hpp"

#include "version.hpp"

#include <dirent.h>

namespace {
    void disp_fast_find_version() { fmt::print("{} {}\n", "fast-find ", version); }
    void copyright() { fmt::print("\n{}\n", "Report bugs or enhancement requests to hungptit@gmail.com"); }
    void usage() {
        fmt::print("\nExamples:\n");
        fmt::print("\t1. Find all files and folders from given paths:\n");
        fmt::print("\t\tfast-find /\n");
        fmt::print("\t2. Find all files that match our desired pattern:\n");
        fmt::print("\t\tfast-find ../src/ -e 'reader'\n");
    }

    auto parse_input_arguments(int argc, char *argv[]) {
        using namespace ioutils::search;
        SearchInputArguments params;
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

        bool ignore_error = false;

        bool version = false;
        std::string begin_time, end_time;

        auto cli =
            clara::Help(help) | clara::Opt(verbose)["-v"]["--verbose"]("Display verbose information.") |
            clara::Help(version) | clara::Opt(version)["--version"]("Display fast-find version.") |
            clara::Opt(ignore_case)["-i"]["--ignore-case"]("Ignore case") |
            clara::Opt(inverse_match)["--invert-match"]("Display paths that do not match a given path regex.") |
            clara::Opt(ignore_file)["--ignore-file"]("Ignore files.") |
            clara::Opt(ignore_dir)["--ignore-dir"]("Ignore folders.") |
            clara::Opt(ignore_symlink)["--ignore-symlink"]("Ignore symlink.") |

            clara::Opt(ignore_fifo)["--ignore-symlink"]("Ignore named pipe (fifo).") |
            clara::Opt(ignore_chr)["--ignore-chr"]("Ignore character special.") |
            clara::Opt(ignore_blk)["--ignore-blk"]("Ignore block special.") |
            clara::Opt(ignore_socket)["--ignore-socket"]("Ignore sockets.") |
            clara::Opt(ignore_whiteout)["--ignore-whiteout"]("Ignore whiteout files.") |

            clara::Opt(donot_ignore_git)["--donot-ignore-git"]("Don't ignore .git folders.") |
            clara::Opt(ignore_error)["--ignore-error"](
                "Don't print out file I/O errors such as \"Permission denied\".") |

            clara::Opt(color)["-c"]["--color"]("Print out color text.") |
            clara::Opt(dfs)["--dfs"]("Use pre-order DFS algorithm for traversing.") |
            clara::Opt(bfs)["--bfs"]("Use BFS algorithm for traversing. Note that "
                                     "BFS algorithm might be slower "
                                     "than DFS algorithm for large folders.") |
            clara::Opt(params.path_regex, "path-regex")["-e"]["-E"]["--regex"]("Search pattern.") |
            clara::Opt(params.maxdepth, "maxdepth")["--maxdepth"]("The maximum search depth.") |

            // Unsupported options
            clara::Opt(follow_link, "follow-link")["--follow-link"]("Follow symbolic links. (WIP)") |
            clara::Opt(begin_time, "newer")["--newer"]("Display paths that are newer than a given timestamp. (WIP)") |
            clara::Opt(end_time, "older")["--older"]("Display paths that are older than a given timestamp. (WIP)") |

            // Required arguments.
            clara::Arg(paths, "paths")("Search paths");

        auto result = cli.parse(clara::Args(argc, argv));
        if (!result) {
            fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
            usage();
            exit(EXIT_FAILURE);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            fmt::print("{}", oss.str());
            usage();
            copyright();
            exit(EXIT_SUCCESS);
        }

        if (version) {
            disp_fast_find_version();
            exit(EXIT_SUCCESS);
        }

        // Need to more extensive path extension here using path_regex.
        if (paths.empty()) {
            params.paths.push_back(".");
        } else {
            for (auto p : paths) {
                params.paths.emplace_back(ioutils::path::simplify_path(p));
            }
        }

        // Init the mode for our regular expression engine.
        params.regex_mode = HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH | (ignore_case ? HS_FLAG_CASELESS : 0);

        // Use BFS if if users want to specify the level of exploration.
        if (params.maxdepth < std::numeric_limits<int>::max()) {
            bfs = true;
        }

        // Select the file traversal algorithm
        dfs = !bfs;

        // Parse the display type
        params.flags =
            ignore_file * FileSearchFlags::IGNORE_FILE | ignore_dir * FileSearchFlags::IGNORE_DIR |
            ignore_symlink * FileSearchFlags::IGNORE_SYMLINK | color * FileSearchFlags::COLOR |
            verbose * FileSearchFlags::VERBOSE | inverse_match * FileSearchFlags::INVERT_MATCH |
            dfs * FileSearchFlags::DFS | ignore_fifo * FileSearchFlags::IGNORE_FIFO |
            ignore_blk * FileSearchFlags::IGNORE_BLK | ignore_chr * FileSearchFlags::IGNORE_CHR |
            ignore_socket * FileSearchFlags::IGNORE_SOCKET | ignore_whiteout * FileSearchFlags::IGNORE_WHITEOUT |
            ignore_unknown * FileSearchFlags::IGNORE_UNKNOWN | donot_ignore_git * FileSearchFlags::DONOT_IGNORE_GIT |
            ignore_error * FileSearchFlags::IGNORE_ERROR;

        // Display input arguments in JSON format if verbose flag is on
        if (params.verbose()) {
            params.print();
        }

        return params;
    }
} // namespace

namespace {
    template <typename Params> void search(Params &&params) {
        if (params.path_regex.empty()) {
            using Policy = ioutils::find::SimplePolicy;
            ioutils::filesystem::DefaultSearch<Policy> search(params);
            search.traverse(params.paths);
        } else {
            if (!params.invert_match()) {
                using Matcher = utils::hyperscan::RegexMatcher;
                using Policy = ioutils::find::RegexPolicy<Matcher>;
                ioutils::filesystem::DefaultSearch<Policy> search(params);
                search.traverse(params.paths);
            } else {
                using Matcher = utils::hyperscan::RegexMatcherInv;
                using Policy = ioutils::find::RegexPolicy<Matcher>;
                ioutils::filesystem::DefaultSearch<Policy> search(params);
                search.traverse(params.paths);
            }
        }
    }
} // namespace

int main(int argc, char *argv[]) {
    search(parse_input_arguments(argc, argv));
    return EXIT_SUCCESS;
}
