#pragma once
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fmt/format.h"

namespace ioutils {
    // This function read the content of a file into a string with the
    // assumption that the file content can be loaded into memory.
    template <typename Container>
    void read(const char *afile, Container &buffer, char *buf, const size_t buffer_size) {
        int fd = ::open(afile, O_RDONLY);

        // Check that we can open a given file.
        if (fd < 0) {
            fmt::MemoryWriter writer;
            writer << "Cannot open file \"" << afile << "\"";
            throw(std::runtime_error(writer.str()));
        }

        // Reserve the size of a buffer using file size information.
        struct stat file_stat;
        if (fstat(fd, &file_stat) < 0) return;
        buffer.reserve(file_stat.st_size);

        // Read data into a string
        while (true) {
            auto nbytes = ::read(fd, buf, buffer_size);
            if (nbytes < 0) {
                fmt::MemoryWriter writer;
                writer << "Cannot read file \"" << afile << "\"";
                throw(std::runtime_error(writer.str()));
            };

            // TODO: Figure out why this function call take 50% of runtime in Linux (SSD
            // drive)?
            buffer.append(buf, nbytes);

            // Stop if we reach the end of file.
            if (nbytes != buffer_size) {
                break;
            };
        }

        // Close our file.
        ::close(fd);
    }

    constexpr size_t READ_TRUNK_SIZE = 1 << 20;
    template <typename Container, size_t BUFFER_SIZE = READ_TRUNK_SIZE>
    void read(const char *afile, Container &buffer) {
        static_assert(READ_TRUNK_SIZE > 128, "READ_TRUNK_SIZE should be greater than 128!");
        char buf[BUFFER_SIZE + 1];
        read(afile, buffer, buf, BUFFER_SIZE);
    }

    template <typename Container, size_t BUFFER_SIZE = READ_TRUNK_SIZE>
    Container read(const char *afile) {
        static_assert(READ_TRUNK_SIZE > 128, "READ_TRUNK_SIZE should be greater than 128!");
        char buf[BUFFER_SIZE + 1];
        Container results;
        read(afile, results, buf, BUFFER_SIZE);
        return results;
    }
} // namespace ioutils
