#pragma once
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#include <string>
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

    template <int SIZE = 1 << 16> struct StreamReaderAppendPolicy {
        char read_buffer[SIZE];
        std::string file_name;
        static constexpr int BUFFER_SIZE = SIZE;
        void set_filename(const char *datafile) { file_name = datafile; }
        std::string data;
        void process(const char *begin, const size_t len) {
            if (len > 0) {
                data.append(begin, len);
            }
        }
    };
} // namespace ioutils
