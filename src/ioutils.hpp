#pragma once
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fmt/format.h"

namespace ioutils {
    class AppendPolicy {
      public:
        void process(const char *buf, const size_t len) { buffer.append(buf, len); }
        std::string data() { return std::move(buffer); }

      private:
        std::string buffer;
    };

    constexpr size_t READ_TRUNK_SIZE = 1 << 16;

    // A reader class which stores the policy as a member data.
    template <typename Policy, size_t BUFFER_SIZE = READ_TRUNK_SIZE>
    struct FileReader : public Policy {
        void operator()(const char *datafile, const long offset = 0) {
            char read_buffer[BUFFER_SIZE + 1];
            int fd = ::open(datafile, O_RDONLY);

            // Check that we can open a given file.
            if (fd < 0) {
                std::stringstream writer;
                writer << "Cannot open file \"" << datafile << "\"";
                throw(std::runtime_error(writer.str()));
            }

            // Shift to desired location if it is not zero.
            if (offset) {
                auto retval = lseek(fd, offset, SEEK_SET);
                if (retval != offset) {
                    std::stringstream writer;
                    writer << "Cannot seek for the location " << offset << " in " << datafile;
                    throw(std::runtime_error(writer.str()));
                }
            }

            // Let the kernel know that we are going to read sequentially to the end of a file.
            // posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

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

    // Return a string which has the content of a file.
    template <size_t BUFFER_SIZE = READ_TRUNK_SIZE> std::string read(const char *afile) {
        static_assert(READ_TRUNK_SIZE > (1 << 10),
                      "READ_TRUNK_SIZE should be greater than 1K!");
        using Reader = FileReader<AppendPolicy, BUFFER_SIZE>;
        Reader reader;
        reader(afile);
        return reader.data();
    }

} // namespace ioutils
