#pragma once

#include "fdreader.hpp"
#include "fdwriter.hpp"
#include "filesystem.hpp"
#include "search.hpp"
#include <fcntl.h>
#include <string>
#include <unistd.h>

#include "memchr.hpp"
#include "regex_matchers.hpp"

#include "hs/hs.h"

namespace ioutils {
    namespace locate {
        struct UpdateDBInputArguments {
            int flags = 0;
            int maxdepth = std::numeric_limits<int>::max();
            bool stats = 0;
            bool verbose = 0;
            std::string database;           // The output fast-locate database path
            std::vector<std::string> paths; // Saearch paths

            bool dfs() { return true; } // Use dfs traversal to explore folders.
            bool donot_ignore_git() const { return true; }
            bool follow_symlink() const { return false; }
            bool ignore_error() const { return false; }

            void print() const {
                fmt::print("verbose: {}\n", verbose);
                fmt::print("statistics: {}\n", stats);
                fmt::print("Search paths: [\"{}\"]\n", fmt::join(paths, "\",\""));
                fmt::print("Database: {}\n", database);
            }
        };

        enum LocateFlags : uint32_t {
            VERBOSE = 1,
            COLOR = 1 << 1,
            INVERT_MATCH = 1 << 2,
            EXACT_MATCH = 1 << 3,
            IGNORE_CASE = 1 << 4,
            INFO = 1 << 5,
            COUNT = 1 << 6,
        };

        struct LocateInputArguments {
            int flags;
            int regex_mode;
            std::string prefix;                 // Prefix of displayed path
            std::string pattern;                // A search pattern
            std::vector<std::string> databases; // A file information database

            bool verbose() const { return (flags & VERBOSE) > 0; }
            bool info() const { return (flags & INFO) > 0; }
            bool invert_match() const { return (flags & INVERT_MATCH) > 0; }
            bool exact_match() const { return (flags & EXACT_MATCH) > 0; }
            bool ignore_case() const { return (flags & IGNORE_CASE) > 0; }

            void print() const {
                fmt::print("verbose: {}\n", verbose());
                fmt::print("info: {}\n", info());
                fmt::print("invert-match: {}\n", invert_match());
                fmt::print("exact-match: {}\n", exact_match());
                fmt::print("ignore-case: {}\n", ignore_case());
                fmt::print("regex-mode: {}\n", regex_mode);
                fmt::print("Search pattern: '{}'\n", pattern);
                fmt::print("path prefix: '{}'\n", prefix);
                fmt::print("File information databases: [\"{}\"]\n", fmt::join(databases, "\",\""));
            }
        };

        struct UpdateDBStreamPolicy {
            template <typename Params> UpdateDBStreamPolicy(Params &&params) : writer(params.database.data()) {}

            bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
            void process_file(const Path &parent, const char *stem = nullptr) {
                writer.write(parent.path.data(), parent.path.size());
                if (stem != nullptr) {
                    writer.sep();
                    writer.write(stem, strlen(stem));
                }
                writer.eol();
            }

            void process_symlink(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

            /**
             * Just store the FIFO paths for now.
             */
            void process_fifo(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

            /**
             * Store the path of character special for now. Not sure if users ever need to
             * know about this type of paths.
             */
            void process_chr(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

            /**
             * Do not have a specific example for this type. Let store its path for now.
             */
            void process_blk(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

            /**
             * Treat socket as file for now. We are generally not interrested in sockets.
             */
            void process_socket(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

            /**
             * A whiteout path is specific to MacOS.
             */
            void process_whiteout(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

            /**
             * TODO: We treat unknown as a file for now. This strategy seem to work well
             * in MacOS.
             */
            void process_unknown(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

            void process_dir(const std::string &path) {
                writer.write(path.data(), path.size());
                writer.eol();
            }

          private:
            StreamWriter writer;
        };
    } // namespace locate

    /**
     * Assumptions:
     * 1. The path length should be less than 1K
     * 2. A path can contain any regular characters.
     */
    template <typename Matcher> class LocateStreamPolicy {
      public:
        static constexpr int BUFFER_SIZE = 1 << 17;
        template <typename Params>
        LocateStreamPolicy(Params &&params)
            : matcher(params.pattern, params.regex_mode), prefix(params.prefix), console(StreamWriter::STDOUT) {
            read_buffer = buffer;
        }

        void process(const char *begin, const size_t len) {
            const char *start = buffer;
            const char *end = begin + len;
            const char *ptr = buffer;
#ifdef USE_AVX2
            while ((ptr = utils::avx2::memchr(ptr, EOL, end - ptr))) {
#else
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
#endif
                process_line(start, ptr - start + 1);
                start = ++ptr;
                if (ptr == end) break;
            }

            if (len < BUFFER_SIZE) {
                process_line(start, end - start);
            } else {
                const size_t leftover_bytes = end - start;
                if (leftover_bytes > 0) {
                    memcpy(buffer, start, leftover_bytes);
                    read_buffer = buffer + leftover_bytes;
                    ;
                }
            }
        }

      protected:
        Matcher matcher;
        std::string prefix;
        StreamWriter console;
        static constexpr char EOL = '\n';
        static constexpr int PATH_MAX_SIZE = 1 << 12;
        char buffer[BUFFER_SIZE + PATH_MAX_SIZE];
        char *read_buffer = nullptr;

        void process_line(const char *begin, const size_t len) {
            if (matcher.is_matched(begin, len)) {
                if (prefix.empty()) {
                    console.write(begin, len);
                } else {
                    console.write(prefix.data(), prefix.size());
                    console.write(begin, len);
                }
            }
        }

        // Note: Override this function to make FileReader happy. We do not care about
        // the database name in fast-locate.
        void set_filename(const char *) {}
    };

    struct PrintAllPolicy {
        template <typename Params> PrintAllPolicy(Params &&args) : prefix(args.prefix), console(StreamWriter::STDOUT) {}
        void process(const char *begin, const size_t len) { console.write(begin, len); }
        void set_filename(const char *) {}

        static constexpr int BUFFER_SIZE = 1 << 17;
        char read_buffer[BUFFER_SIZE];
        std::string prefix = "";
        StreamWriter console;
    };

    void updatedb(ioutils::locate::UpdateDBInputArguments &params) {
        using Policy = ioutils::locate::UpdateDBStreamPolicy;
        using Search = typename ioutils::filesystem::DefaultSearch<Policy>;
        Search search(params);
        search.traverse(params.paths);
    }

    template <typename Matcher, typename Params> void find_matched_files(Params &&params) {
        using GrepAlg = ioutils::StreamReader<ioutils::LocateStreamPolicy<Matcher>>;
        GrepAlg grep(params);
        for (auto db : params.databases) {
            grep(db.data());
        }
    }

    void locate_files(const ioutils::locate::LocateInputArguments &params) {
        if (params.pattern.empty()) {
            using GrepAlg = ioutils::StreamReader<ioutils::PrintAllPolicy>;
            GrepAlg grep(params);
            for (auto db : params.databases) {
                grep(db.data());
            }
        } else {
            if (!params.invert_match()) {
                using Matcher = utils::hyperscan::RegexMatcher;
                find_matched_files<Matcher>(params);
            } else {
                using Matcher = utils::hyperscan::RegexMatcherInv;
                find_matched_files<Matcher>(params);
            }
        }
    }
} // namespace ioutils
