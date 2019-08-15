#pragma once

#include "fdwriter.hpp"
#include "filesystem.hpp"
#include "search.hpp"
#include "utils/memchr.hpp"
#include <fcntl.h>
#include <string>
#include <unistd.h>
namespace ioutils {
    namespace locate {
        struct UpdateDBStreamPolicy {
          public:
            UpdateDBStreamPolicy() : writer(StreamWriter::STDOUT) {}

            template <typename Params> UpdateDBStreamPolicy(Params &&params) : writer(params.database.data()) {}

          protected:
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
             * Store the path of character special for now. Not sure if users ever need to know about this type of
             * paths.
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
             * TODO: We treat unknown as a file for now. This strategy seem to work well in MacOS.
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
     * 1. The path length should be less than 1K
     * 2. A path can contain any regular characters.
     * 3.
     *
     */
    template <typename Matcher> class LocateStreamPolicy {
      public:
        static constexpr int BUFFER_SIZE = 1 << 17;
        static constexpr int PATH_MAX_SIZE = 1 << 12;
        template <typename Params>
        LocateStreamPolicy(Params &&params)
            : matcher(params.pattern, params.regex_mode),
              prefix(params.prefix),
              console(StreamWriter::STDOUT) {
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
                    read_buffer = buffer + leftover_bytes;;
                }
            }
        }

      protected:
        Matcher matcher;
        std::string prefix;
        StreamWriter console;
        static constexpr char EOL = '\n';
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

        // Note: Override this function to make FileReader happy. We do not care about the database name in
        // fast-locate.
        void set_filename(const char *) {}
    };

    class PrintAllPolicy {
      public:
        template <typename Params> PrintAllPolicy(Params &&args) : prefix(args.prefix), console(StreamWriter::STDOUT) {}
        void process(const char *begin, const size_t len) { console.write(begin, len); }

      protected:
        void set_filename(const char *) {}
        static constexpr int BUFFER_SIZE = 1 << 17;
        char read_buffer[BUFFER_SIZE];

      private:
        std::string prefix;
        StreamWriter console;
    };
} // namespace ioutils
