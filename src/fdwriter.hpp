#pragma once

#include <cstring>
#include <fcntl.h>
#include <string>
#include <unistd.h>

namespace ioutils {
    class Console {
      public:
        explicit Console() : maxlen(1 << 12) { buffer.reserve(maxlen); }
        explicit Console(const size_t len) : maxlen(len) { buffer.reserve(maxlen); }
        size_t print(const char *p, const size_t len) {
            buffer.append(p, len);
            if (buffer.size() > maxlen) {
                return ::write(STDOUT, buffer.data(), buffer.size());
            }
            return len;
        }

        ~Console() { flush(); }

        // Flush all data in the buffer.
        size_t flush() {
            size_t nbytes = ::write(STDOUT, buffer.data(), buffer.size());
            buffer.clear();
            return nbytes;
        }

      private:
        size_t maxlen;
        std::string buffer;
        static constexpr int STDOUT = 0;
        static constexpr int STDERR = 1;
    };
} // namespace ioutils
