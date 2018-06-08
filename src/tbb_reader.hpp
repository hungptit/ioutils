#pragma once

#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// TBB
#include "tbb/task_group.h"

namespace ioutils {
    // A reader class which stores the policy as a member data.
    template <typename Policy, int BUFFER_SIZE = 1 << 16> struct TBBFileReader : public Policy {
        TBBFileReader() : Policy() {}
        TBBFileReader(const std::string &pattern) : Policy(pattern) {}

        void operator()(const char *datafile) {
            char read_buffer[BUFFER_SIZE];
            char process_buffer[BUFFER_SIZE];
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);

            // Check that we can open a given file.
            if (fd < 0) {
                const std::string msg = std::string("Cannot open file \"") + datafile + "\"";
                throw(std::runtime_error(msg));
            }

            // Used lambda objects
            auto produce = [datafile](int fd, char *buffer, const int buffer_size) {
                int nbytes = ::read(fd, buffer, buffer_size);
                if (nbytes < 0) {
                    const std::string msg =
                        std::string("Cannot read file \"") + datafile + "\"";
                    throw(std::runtime_error(msg));
                };
            };

            auto consume = [](const char *buffer, const int buffer_size) {
                Policy::process(buffer, buffer_size);
            };

            // Read data into a read buffer
            int read_bytes = 0;
            int process_bytes = 0;
            while (true) {
                read_bytes = produce(fd, read_buffer, BUFFER_SIZE);
                consume(read_buffer, read_bytes);

                // Update data for the next step
                process_bytes = read_bytes;

                // Stop if we reach the end of file.
                if (read_bytes != BUFFER_SIZE) {
                    break;
                };
            }

            // Close our file.
            ::close(fd);

            // Consume the read buffer if there is any data left.
            Policy::process(read_buffer, read_bytes);
        }
    };

} // namespace ioutils
