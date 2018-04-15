#pragma once
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fmt/format.h"

namespace ioutils {
    struct AppendPolicy {
        void operator()(const char *buffer, const size_t len) { _data.append(buffer, len); }
		std::string data() {return std::move(_data);}
        std::string _data;
    };

	constexpr size_t READ_TRUNK_SIZE = 1 << 16;
	
    // A reader class which stores the policy as a member data.
    template <typename Policy, size_t BUFFER_SIZE = READ_TRUNK_SIZE> struct FileReader {
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
                policy(read_buffer, nbytes);

                // Stop if we reach the end of file.
                if (nbytes != static_cast<decltype(nbytes)>(BUFFER_SIZE)) {
                    break;
                };
            }

            // Close our file.
            ::close(fd);
        }

        // Member data
        Policy policy;
    };

    // A reader class which parse data using a given policy.
    template <typename Policy, size_t BUFFER_SIZE = READ_TRUNK_SIZE> class FileReader2 {
      public:
        void operator()(const char *datafile, Policy &policy, const long offset = 0) {
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

            // Read data into a string
            while (true) {
                auto nbytes = ::read(fd, read_buffer, BUFFER_SIZE);
                if (nbytes < 0) {
                    std::stringstream writer;
                    writer << "Cannot read file \"" << datafile << "\"";
                    throw(std::runtime_error(writer.str()));
                };

                // Parse read_buffer to get some useful information.
                policy(read_buffer, nbytes);

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
        static_assert(READ_TRUNK_SIZE > 128, "READ_TRUNK_SIZE should be greater than 128!");
		using Reader = FileReader<AppendPolicy, BUFFER_SIZE>;
		Reader reader;
		reader(afile);
		return std::string();
		// return reader.policy.data();
    }

} // namespace ioutils
