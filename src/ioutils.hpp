#pragma once
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "fmt/format.h"
#include <thread>
#include <mutex>
#include <deque>

namespace ioutils {
    class AppendPolicy {
      public:
        void process(const char *buffer, const size_t len) { _data.append(buffer, len); }
        std::string data() { return std::move(_data); }

      private:
        std::string _data;
    };

    constexpr size_t READ_TRUNK_SIZE = 1 << 16;

    // A reader class which stores the policy as a member data.
    template <typename Policy, size_t BUFFER_SIZE = READ_TRUNK_SIZE>
    struct FileReader : public Policy {
        FileReader() : Policy() {}
        FileReader(const std::string &pattern) : Policy(pattern) {}

        void operator()(const char *datafile) {
            char read_buffer[BUFFER_SIZE];
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);

            // Check that we can open a given file.
            if (fd < 0) {
                const std::string msg = std::string("Cannot open file \"") + datafile + "\"";
                throw(std::runtime_error(msg));
            }

            // Read data into a read buffer
            while (long nbytes = ::read(fd, read_buffer, BUFFER_SIZE)) {
                if (nbytes < 0) {
                    std::stringstream writer;
                    writer << "Cannot read file \"" << datafile << "\" "
                           << "nbytes = " << nbytes;
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

    // Read data using readv
    template <typename Policy, size_t BUFFER_SIZE = READ_TRUNK_SIZE>
    struct FileReaderNew : public Policy {
        FileReaderNew() : Policy() {}
        FileReaderNew(const std::string &pattern) : Policy(pattern) {}

        void operator()(const char *datafile) {
            char read_buffer[BUFFER_SIZE];
            struct iovec iov;
            iov.iov_base = read_buffer;
            iov.iov_len = BUFFER_SIZE;

            // Open an input file in read only mode.
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);
            if (fd < 0) {
                const std::string msg = std::string("Cannot open file \"") + datafile + "\"";
                throw(std::runtime_error(msg));
            }

            // Read data into a read buffer
            while (ssize_t nbytes = ::readv(fd, &iov, 1)) {
                Policy::process(read_buffer, nbytes);
                if (nbytes != static_cast<decltype(nbytes)>(BUFFER_SIZE)) {
                    break;
                };
            }

            // Close our file.
            ::close(fd);
        }
    };

    // Read data using readv
    template <typename Policy, size_t BUFFER_SIZE = READ_TRUNK_SIZE>
    struct ProducerConsumer : public Policy {
        ProducerConsumer() : Policy() {}
        ProducerConsumer(const std::string &pattern) : Policy(pattern) {}

        void operator()(const char *datafile) {
            producer(datafile);
            // consumer();
        }

        void producer(const char *datafile) {
            char read_buffer[BUFFER_SIZE];

            // Open an input file in read only mode.
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);
            if (fd < 0) {
                const std::string msg = std::string("Cannot open file \"") + datafile + "\"";
                throw(std::runtime_error(msg));
            }

            while (long nbytes = ::read(fd, read_buffer, BUFFER_SIZE)) {
                if (nbytes < 0) {
                    std::stringstream writer;
                    writer << "Cannot read file \"" << datafile << "\" "
                           << "nbytes = " << nbytes;
                    throw(std::runtime_error(writer.str()));
                };

                // Apply a given policy to read_buffer.
                if (nbytes) {
                    std::string read_data(read_buffer, nbytes);
                    queue.emplace_back(read_data);
                }

                // Stop if we reach the end of file.
                if (nbytes != static_cast<decltype(nbytes)>(BUFFER_SIZE)) {
                    break;
                };
            }

            // Close our file.
            ::close(fd);
        }

        void consumer() {
            while (true) {
                if (!queue.empty()) {
                    auto data = queue.front();
                    Policy::process(data.data(), data.size());
                    queue.pop_front();
                } else {
                    if (!isok) {
                        break;
                    }
                }
            }
        }

      protected:
        std::mutex lock;
        bool isok = true;
        std::deque<std::string> queue;
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
