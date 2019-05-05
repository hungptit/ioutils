#pragma once

#include "fmt/format.h"
#include <stdio.h>
#include <unistd.h>

namespace ioutils {
    template <typename Policy, size_t BUFFER_SIZE = 1 << 16>
    struct StreamReader : public Policy {
        // Need to support different policies.
        template <typename... Args>
        StreamReader(Args... args) : Policy(std::forward<Args>(args)...) {}

        // For stdin use STDIN_FILENO
        void operator()(int fd) {
            char read_buffer[BUFFER_SIZE];
            long nbytes;
            while ((nbytes = ::read(fd, read_buffer, BUFFER_SIZE)) > 0) {
                Policy::process(read_buffer, nbytes);
            }
            if (nbytes < 0) {
                fmt::print(stderr, "Cannot read data from given file descriptor: {}\n",
                           strerror(errno));
            }
        }
    };
} // namespace ioutils
