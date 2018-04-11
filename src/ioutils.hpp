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
            std::string errmsg("Cannot open file \"");
			errmsg.append(afile, "\"");
            throw(std::runtime_error(errmsg));
        }

        // Reserve the size of a buffer using file size information.
        struct stat file_stat;
        if (fstat(fd, &file_stat) < 0) return;
        buffer.reserve(file_stat.st_size);

        // Read data into a string
        while (true) {
            auto nbytes = ::read(fd, buf, buffer_size);
            if (nbytes < 0) {
				std::string errmsg("Cannot read file \"");
				errmsg.append(afile, "\"");
				throw(std::runtime_error(errmsg));
            };

            // TODO: Figure out why this function call take 50% of runtime in Linux (SSD
            // drive)?
            buffer.append(buf, nbytes);

            // Stop if we reach the end of file.
            if (nbytes != static_cast<decltype(nbytes)>(buffer_size)) {
                break;
            };
        }

        // Close our file.
        ::close(fd);
    }

    constexpr size_t READ_TRUNK_SIZE = 1 << 16;
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

	// A struct that read file content in fixed size chunks and parse them to a parser.
    template <size_t BUFFER_SIZE, typename Policy> struct FileReader {
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
                if (nbytes != static_cast<decltype(nbytes)>(BUFFER_SIZE)) { break; };
            }

            // Close our file.
            ::close(fd);
        }

		// Member data
		Policy policy;
    };

	    // A struct that read file content in fixed size chunks and parse them to a parser.
    template <size_t BUFFER_SIZE, typename Parser> class FileReader2 {
      public:
        void operator()(const char *datafile, Parser &parser, const long offset = 0) {
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
                parser(read_buffer, nbytes);

                // Stop if we reach the end of file.
                if (nbytes != static_cast<decltype(nbytes)>(BUFFER_SIZE)) { break; };
            }

            // Close our file.
            ::close(fd);
        }
    };
} // namespace ioutils
