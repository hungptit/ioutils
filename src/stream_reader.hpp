#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {
    /**
     * This class will handle all required buffer tasks. Policy::process method only need to return the number of
     * processed bytes. Left over bytes will be moved to the front of the buffer.
     *
     * Note: 2^16 is the optimum buffer size which is obtained from our performance benchmark results.
     */
    template <typename Policy, size_t BUFFER_SIZE = 1 << 16> struct FileStreamReader : public Policy {
        template <typename... Args> FileStreamReader(Args... args) : Policy(std::forward<Args>(args)...) {}

        void operator()(const char *datafile) {
            // Cache the file name
            Policy::set_filename(datafile);

            // Read data by trunks
            char read_buffer[2 * BUFFER_SIZE];
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);

            // Check that we can open a given file.
            if (fd < 0) {
                fprintf(stderr, "Cannot open file: '%s'. Error: %s\n", datafile, strerror(errno));
                return;
            }

            // Get file size.
            struct stat buf;
            fstat(fd, &buf);
            auto file_size = buf.st_size;

            /**
             * Split into two scenarios:
             * 1. The file content is smaller than our buffer. This assumption may be valid for most situations.
             * 2. The file content cannot be fitted into our buffer. There are two possible situations that we need to
             * handle:
             *
             * a. The maximum line size is smaller than our buffer: We only need to move data to the beginning
             *    of the buffer and read new data.
             *
             * b. The maximum line size if buffer than our buffer: We need to cache
             *    data into a dynamically allocated memory i.e std::string.
             *
             * Note: (1) and (2.a) will cover most of our use cases.
             */
            auto print_error = [](const char *fname) {
                fprintf(stderr, "Cannot read from file '%s'. Error: %s\n", fname, strerror(errno));
            };
            if (file_size <= (2 * BUFFER_SIZE)) {
                long nbytes = ::read(fd, read_buffer, 2 * BUFFER_SIZE);
                nbytes < 0 ? print_error(datafile)
                           : Policy::process(read_buffer, nbytes, true); // All data are fitted into our buffer.
            } else {
                const size_t block_count = (buf.st_size / BUFFER_SIZE) + (buf.st_size % BUFFER_SIZE != 0);
                const char *begin = read_buffer;
                for (size_t blk = 0; blk < block_count; ++blk) {
                    long nbytes = ::read(fd, (void *)begin, BUFFER_SIZE);
                    if (nbytes < 0) {
                        print_error(datafile);
                        break;
                    };
                    auto processed_bytes = Policy::process(read_buffer, nbytes, BUFFER_SIZE);
                }

                Policy::finalize(); // Clear policy's states.
            }

            // Close our file.
            ::close(fd);
        }
    };
} // namespace ioutils
