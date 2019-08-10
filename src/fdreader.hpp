#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {
    /**
     * This class will read data from a given file descriptor in chunks.
     *
     */
    template <typename Policy> struct StreamReader : public Policy {
        template <typename... Args> StreamReader(Args... args) : Policy(std::forward<Args>(args)...) {}

        void operator()(int fd, const char *datafile) {
            char read_buffer[Policy::BUFFER_SIZE];
            Policy::set_filename(datafile);
            while (true) {
                long nbytes = ::read(fd, read_buffer, Policy::BUFFER_SIZE);
                if (nbytes < 0) {
                    fprintf(stderr, "Cannot read from file '%s'. Error: %s\n", datafile, strerror(errno));
                    break;
                };
                Policy::process(read_buffer, nbytes);
                if (nbytes < Policy::BUFFER_SIZE) break;
            }
        }

        void operator()(const char *datafile) {
            char read_buffer[Policy::BUFFER_SIZE];

            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);

            // Check that we can open a given file.
            if (fd < 0) {
                fprintf(stderr, "Cannot open file: '%s'. Error: %s\n", datafile, strerror(errno));
                return;
            }

            // Read data from a give file.
            Policy::set_filename(datafile);
            while (true) {
                long nbytes = ::read(fd, read_buffer, Policy::BUFFER_SIZE);
                if (nbytes < 0) {
                    fprintf(stderr, "Cannot read from file '%s'. Error: %s\n", datafile, strerror(errno));
                    break;
                };
                Policy::process(read_buffer, nbytes);
                if (nbytes < Policy::BUFFER_SIZE) break;
            }
        }
    };
} // namespace ioutils
