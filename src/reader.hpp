#pragma once
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {
    constexpr size_t READ_TRUNK_SIZE = 1 << 16;
    // A reader class which stores the policy as a member data.
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

            // Read data into a read buffer
            while (true) {
                auto nbytes = ::read(fd, read_buffer, BUFFER_SIZE);
                if (nbytes < 0) {
                    std::stringstream writer;
                    writer << "Cannot read file \"" << datafile << "\"";
                    throw(std::runtime_error(writer.str()));
                };

                // Apply a given policy to read_buffer.
                Policy::process(read_buffer, nbytes);

                // Stop if we reach the end of file.
                if (nbytes != static_cast<decltype(nbytes)>(BUFFER_SIZE)) {
                    break;
                };
            }

            // Close our file.
            ::close(fd);
        }
    };

} // namespace ioutils
