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

        StreamWriter(const int fides, const size_t len = BUFFER_SIZE) : fd(fides), buflen(len) {
            buffer.reserve(buflen);
        }

        ~StreamWriter() {
            if (!buffer.empty()) {
                ::write(fd, buffer.data(), buffer.size());
            }
        }

        size_t write(const char *begin, const size_t len) {
            size_t nbytes = len;
            if ((buffer.size() + len) > BUFFER_SIZE) {
                nbytes = ::write(fd, buffer.data(), buffer.size());
                buffer.clear();
            }
            if (len < BUFFER_SIZE) {
                buffer.append(begin, len);
            } else {
                nbytes += ::write(fd, begin, len);
            }
            return nbytes;
        }

      private:
        const int fd;
        int buflen;
        std::string buffer;
    };
} // namespace ioutils
