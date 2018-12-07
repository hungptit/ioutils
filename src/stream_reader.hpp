#pragma once

#include <unistd.h>

namespace ioutils {
    template <typename Policy, size_t BUFFER_SIZE = 1 << 16>
    struct StreamReader : public Policy {

        template <typename... Args>
        StreamReader(Args... args) : Policy(std::forward<Args>(args)...) {}

        /**
         * For stdin use STDIN_FILENO
         */
        void operator()(int fd) {
            char read_buffer[BUFFER_SIZE + 1];
            while (true) {
                long nbytes = ::read(fd, read_buffer, BUFFER_SIZE);
                if (nbytes < 1) break;
                Policy::process(read_buffer, nbytes);
                if (nbytes != BUFFER_SIZE) break;
            }
        }
    };
} // namespace ioutils
