#pragma once
#include "warnings.hpp"
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/errno.h>
#include <unistd.h>

namespace ioutils {
    class StreamWriter {
      public:
        static constexpr int STDOUT = STDOUT_FILENO;
        static constexpr int STDERR = STDERR_FILENO;
        static constexpr size_t BUFFER_SIZE = 1 << 17;

        StreamWriter(const int fides = STDOUT_FILENO, const int len = BUFFER_SIZE)
            : fd(fides), buflen(len) {
            buffer.reserve(buflen);
        }

        StreamWriter(const char *fname, const int len = BUFFER_SIZE) {
            fd = ::open(fname, O_CREAT | O_WRONLY, S_IRWXU);
            if (fd < 0) {
                const std::string msg = "Cannot open file " + std::string(fname) + " to write.";
                throw std::runtime_error(msg);
            }
            buflen = len;
            buffer.reserve(buflen);
        }

        ~StreamWriter() {
            flush();
            if (fd > STDERR) {
                ::close(fd); // Close the output file if needed.
            }
        }

        void write(const char *begin, const size_t len) {
            if ((buffer.size() + len) > BUFFER_SIZE) {
                flush();
            }
            buffer.append(begin, len);
        }

        void flush() {
            if (!buffer.empty()) {
                long nbytes = ::write(fd, buffer.data(), buffer.size());
                if (nbytes != (long)buffer.size()) {
                    handle_error();
                }
                buffer.clear();
            }
        }

        void put(const char ch) { buffer.append(1, ch); }
        void eol() { buffer.append(1, EOL); }
        void sep() { buffer.append(1, SEP); }

      private:
        static constexpr char EOL = '\n';
        static constexpr char SEP = '/';
        int fd;
        int buflen;
        std::string buffer;

        static void handle_error() {
            int errcode = errno;
            if (errcode != 0) {
                std::string errmsg(strerror(errcode));
                long nbytes = ::write(STDERR, errmsg.data(), errmsg.size());
                ignore(nbytes);
            }
        }
    };
} // namespace ioutils
