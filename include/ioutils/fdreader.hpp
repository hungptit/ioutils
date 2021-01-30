#pragma once
#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

namespace ioutils {
    template <typename Policy> struct StreamReader : public Policy {
        template <typename... Args> StreamReader(Args... args) : Policy(std::forward<Args>(args)...) {}

        // Read data from a given file descriptor in chunks.
        void read(int fd, const char *datafile) {
            Policy::set_filename(datafile);
            while (true) {
                long nbytes = ::read(fd, Policy::read_buffer, Policy::BUFFER_SIZE);
                if (nbytes < 0) {
                    fprintf(stderr, "Cannot read from file '%s'. Error: %s\n", datafile, strerror(errno));
                    break;
                };
                Policy::process(Policy::read_buffer, nbytes);
                if (nbytes < Policy::BUFFER_SIZE) break;
            }
        }

        void operator()(const char *datafile) {
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);
            if (fd < 0) {
                fprintf(stderr, "Cannot open file: '%s'. Error: %s\n", datafile, strerror(errno));
                return;
            }
            read(fd, datafile);
        }
    };
} // namespace ioutils

