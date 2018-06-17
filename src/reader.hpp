#pragma once
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {

    // A reader class which reads data in blocks.
    constexpr size_t READ_TRUNK_SIZE = 1 << 17;
    template <typename Policy, size_t BUFFER_SIZE = READ_TRUNK_SIZE>
    struct FileReader : public Policy {
        FileReader() : Policy() {}
        FileReader(const std::string &pattern) : Policy(pattern) {}

        void operator()(const char *datafile) {
            char read_buffer[BUFFER_SIZE + 1];
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);

            // Check that we can open a given file.
            if (fd < 0) {
                const std::string msg = std::string("Cannot open file \"") + datafile + "\"";
                throw(std::runtime_error(msg));
            }

            // Get file size.
            struct stat buf;
            fstat(fd, &buf);

            // Read data into a read buffer
            size_t block_count = (buf.st_size / BUFFER_SIZE) + (buf.st_size % BUFFER_SIZE != 0);
            for (size_t blk = 0; blk < block_count; ++blk) {
                long nbytes = ::read(fd, read_buffer, BUFFER_SIZE);
                if (nbytes < 0) {
                    const std::string msg =
                        std::string("Cannot read from file \"") + std::string(datafile) + "\" ";
                    throw(std::runtime_error(msg));
                };

                // Apply a given policy to read_buffer.
                Policy::process(read_buffer, nbytes);
            }

            // Close our file.
            ::close(fd);
        }
    };

    // A reader that use memory mapped approach.
    template <typename Policy> struct MMapReader : public Policy {
        MMapReader() : Policy() {}
        MMapReader(const std::string &pattern) : Policy(pattern) {}
        void operator()(const char *datafile) {
            // Error handling object.
            auto handle_error = [datafile](const std::string &errmsg) {
                const std::string msg = std::string("Cannot open ") + datafile;
                throw(msg);
            };

            // Open data file for reading
            int fd = open(datafile, O_RDONLY);
            if (fd == -1) {
                handle_error("Cannot open ");
            }

            // Obtain file size
            struct stat info;
            if (fstat(fd, &info) == -1) {
                handle_error("Cannot get information of ");
            }
            size_t length = info.st_size;

            // Create mapped memory
            // const int flags = MAP_PRIVATE;
            const int flags = MAP_PRIVATE | MAP_POPULATE;
            char *begin =
                static_cast<char *>(mmap(nullptr, length, PROT_READ, flags, fd, 0u));

            if (begin == MAP_FAILED) {
                handle_error("Cannot map ");
            }

            // Tell the kernel that we will access file content sequentially.
            if (madvise(begin, length, MADV_SEQUENTIAL | MADV_WILLNEED) != 0) {
                const std::string msg("Cannot set hint for kernel!");
                auto nbytes = write(1, msg.data(), msg.size());
            }

            // Process mapped buffer
            Policy::process(begin, length);

            // Cleanup
            munmap(begin, length);
            close(fd);
        }
    };

} // namespace ioutils
