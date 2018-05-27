#include "fmt/format.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace {
    // Read file using memory mapped approach.
    template <typename Container> Container read_memmap(const std::string &afile) {
        boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
        auto begin = mmap.const_data();
        auto end = begin + mmap.size();
        return Container(begin, end);
    }

    // Read from file in chunks
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
            char read_buffer[BUFFER_SIZE + 1];
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);

            // Check that we can open a given file.
            if (fd < 0) {
                std::stringstream writer;
                writer << "Cannot open file \"" << datafile << "\"";
                throw(std::runtime_error(writer.str()));
            }

            // Let the kernel know that we are going to read sequentially to the end of a file.
            // posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

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

} // namespace

int main(const int argc, char *argv[]) {
    if (argc < 3) {
        fmt::print("Need to provide an option and a file name\n");
    }

    std::string option(argv[1]);
    std::string fname(argv[2]);
    if (option == "boost") {
        auto data = read_memmap<std::vector<char>>(fname);
        fmt::print("Number of bytes: {}\n", data.size());
    } else if (option == "chunk") {
        using Reader = FileReader<AppendPolicy, 1 << 16>;
        Reader reader;
        reader(fname.c_str());
        fmt::print("Number of bytes: {}\n", reader.data().size());
    }
}
