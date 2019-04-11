#pragma once
#include "fmt/format.h"
#include <cstring>
#include <fcntl.h>
#include <string>
#include <unistd.h>

namespace ioutils {
    class StreamWriter {
      public:
        static constexpr int STDOUT = STDOUT_FILENO;
        static constexpr int STDERR = STDERR_FILENO;
        static constexpr size_t BUFFER_SIZE = 1 << 16;

        static StreamWriter stdout() { return StreamWriter(STDOUT); }
        static StreamWriter stderr() { return StreamWriter(STDERR); }

        StreamWriter(const int fides, const int len = BUFFER_SIZE) : fd(fides), buflen(len) {
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
            if (!buffer.empty()) {
                ::write(fd, buffer.data(), buffer.size());
            }

            // Close the output file if needed.
            if (fd > STDERR) {
                ::close(fd);
            }
        }

        void write(const char *begin, const size_t len) {
            // Flush the buffer it reaches the threshold.
            if ((buffer.size() + len) > BUFFER_SIZE) {
                long nbytes = ::write(fd, buffer.data(), buffer.size());
                if (nbytes < 0) {
                    throw std::runtime_error("Cannot write to the output file descriptor.");
                }
                buffer.clear();
            }

            // Append data to the buffer.
            buffer.append(begin, len);
        }

        void put(const char ch) {
            buffer.push_back(ch);
        }

      private:
        int fd;
        int buflen;
        std::string buffer;
    };
} // namespace ioutils
