#pragma once

#include <cstring>
#include <fcntl.h>
#include <string>
#include <unistd.h>

namespace ioutils {
    namespace bash {
        enum TextStyle : int8_t { NORMAL, BOLD, UNDERLINE };
        enum Color : int8_t { BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE };

        enum ForegroundColor : int8_t {
            BLACK_FOREGROUND = 30,
            RED_FOREGROUND = 31,
            GREEN_FOREGROUND = 32,
            YELLOW_FOREGROUND = 33,
            BLUE_FOREGROUND = 34,
            PURPLE_FOREGROUND = 35,
            CYAN_FOREGROUND = 36,
            WHITE_FOREGROUND = 37,
        };

        template <typename Style, typename ForegroundColor, typename BackgroundColor>
        struct TextFormat;
    } // namespace bash

    // Simple writer.
    class Writer {
      public:
        Writer() : fildes(STDOUT_FILENO) {} // Write to stdout by default.
        Writer(int fd) : fildes(fd) {}

        size_t write(const char *begin, const size_t len) {
            return ::write(fildes, begin, len);
        }

        size_t write(const std::string &data) { return write(data.data(), data.size()); }

      private:
        int fildes;
    };

    // Buffered writer.
    class BufferedWriter {
      public:
        BufferedWriter(const size_t buflen = BUFFER_SIZE) : fildes(STDOUT_FILENO) {
            buffer.reserve(buflen);
        }

        BufferedWriter(int fd, const size_t buflen = BUFFER_SIZE) : fildes(fd) {
            buffer.reserve(buflen);
        }

        size_t write(const char *begin, const size_t len) {
            size_t nbytes = len;
            if (buffer.size() > BUFFER_SIZE) {
                nbytes = ::write(fildes, begin, len);
                buffer.clear();
            }
            buffer.append(begin, len);

            return nbytes;
        }

        // Flush all data in the buffer.
        size_t flush() {
            auto nbytes = ::write(fildes, buffer.data(), buffer.size());
            buffer.clear();
            return nbytes;
        }

      private:
        static constexpr size_t BUFFER_SIZE = 1 << 16;
        int fildes;
        std::string buffer;
    };
} // namespace ioutils
