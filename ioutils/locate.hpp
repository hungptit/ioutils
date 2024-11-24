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

namespace ioutils {
    namespace locate {
        enum LocateFlags : uint32_t {
            VERBOSE = 1,
            COLOR = 1 << 1,
            INVERT_MATCH = 1 << 2,
            EXACT_MATCH = 1 << 3,
            IGNORE_CASE = 1 << 4,
            INFO = 1 << 5,
            COUNT = 1 << 6,
        };
    }

    struct LocateInputArguments {
        int flags;
        int regex_mode;
        std::string prefix;                 // Prefix of displayed path
        std::string pattern;                // A search pattern
        std::vector<std::string> databases; // A file information database

        [[nodiscard]] auto verbose() const -> bool { return (flags & VERBOSE) > 0; }
        [[nodiscard]] auto info() const -> bool { return (flags & locate::INFO) > 0; }
        [[nodiscard]] auto invert_match() const -> bool { return (flags & locate::INVERT_MATCH) > 0; }
        [[nodiscard]] auto exact_match() const -> bool { return (flags & locate::EXACT_MATCH) > 0; }
        [[nodiscard]] auto ignore_case() const -> bool { return (flags & locate::IGNORE_CASE) > 0; }

        void print() const;
    };

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

    template <typename Matcher, typename Params> void find_matched_files(Params &&params) {
        using GrepAlg = ioutils::StreamReader<LocateStreamPolicy<Matcher>>;
        GrepAlg grep(params);
        for (auto db : params.databases) {
            grep(db.data());
        }
    }

    void locate_files(const ioutils::LocateInputArguments &params);
} // namespace ioutils
