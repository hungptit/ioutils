#pragma once
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/errno.h>
#include <unistd.h>

namespace ioutils {
    class StreamWriter {
      public:
        static constexpr int STDOUT = STDOUT_FILENO;
        static constexpr int STDERR = STDERR_FILENO;
        static constexpr size_t BUFFER_SIZE = 1 << 16;

        static StreamWriter stdout() { return StreamWriter(STDOUT); }
        static StreamWriter stderr() { return StreamWriter(STDERR); }

        StreamWriter(const int fides, const int len = BUFFER_SIZE) : fd(fides), buflen(len) { buffer.reserve(buflen); }

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
            if (!buffer.empty()) {
                auto nbytes = ::write(fd, buffer.data(), buffer.size());
                if (nbytes != buffer.size()) {
                    handle_error();
                }
            }

            // Close the output file if needed.
            if (fd > STDERR) {
                ::close(fd);
            }
        }

        void write(const char *begin, const size_t len) {
            // Flush the buffer it reaches the threshold.
            if ((buffer.size() + len) > BUFFER_SIZE) {
                auto nbytes = ::write(fd, buffer.data(), buffer.size());
                if (nbytes != buffer.size()) {
                    handle_error();
                }
                buffer.clear();
            }

            // Append data to the buffer.
            buffer.append(begin, len);
        }

        void put(const char ch) { buffer.push_back(ch); }

        void eol() {
            buffer.push_back(EOL);
        }

        void sep() {
            buffer.push_back(SEP);
        }
        
      private:
        const char EOL = '\n';
        const char SEP = '/';
        int fd;
        int buflen;
        std::string buffer;

        void handle_error() {
            int errcode = errno;
            if (errcode) {
                std::string errmsg(strerror(errcode));
                ::write(STDERR, errmsg.data(), errmsg.size());
            }
        }
    };
} // namespace ioutils
