#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {
    /**
     * A reader that use memory mapped approach. This approach is slower than streaming approach
     * in general.
     */
    template <typename Policy> struct MMapReader : public Policy {
        MMapReader() : Policy() {}

        template <typename... Args> MMapReader(Args... args) : Policy(std::forward<Args>(args)...) {}

        void operator()(const char *datafile) {
            // Cache the file name.
            Policy::set_filename(datafile);

            // Open data file for reading
            int fd = open(datafile, O_RDONLY);
            if (fd == -1) {
                fprintf(stderr, "Cannot open file '%s'\n", datafile);
                return;
            }

            // Obtain file size
            struct stat info;
            if (fstat(fd, &info) == -1) {
                fprintf(stderr, "Cannot get information of '%s'\n", datafile);
            }
            size_t length = info.st_size;

            // If a given file is empty then return early.
            if (length == 0) {
                close(fd);
                return;
            }

            // Create mapped memory
            const int flags = MAP_PRIVATE;
            // const int flags = MAP_PRIVATE | MAP_POPULATE;
            char *begin = static_cast<char *>(mmap(nullptr, length, PROT_READ, flags, fd, 0u));

            if (begin == MAP_FAILED) {
                fprintf(stderr, "Cannot map '%s'\n", datafile);
                return;
            }

            // Tell the kernel that we will access file content sequentially.
            if (madvise(begin, length, MADV_SEQUENTIAL | MADV_WILLNEED) != 0) {
                fprintf(stderr, "Cannot set hint for kernel!");
            }

            // Process mapped buffer
            Policy::process(begin, length, length);

            // Cleanup
            munmap(begin, length);
            close(fd);
        }
    };
} // namespace ioutils
