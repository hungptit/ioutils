#include "fmt/format.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "boost/coroutine2/all.hpp"
#include <atomic>
#include <mutex>
#include <thread>

#include "ioutils.hpp"
#include "linestats.hpp"

#include "celero/Celero.h"

namespace {
    constexpr char EOL('\n');
    // Read file using memory mapped approach.
    size_t read_memmap(const std::string &afile) {
        boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
        auto begin = mmap.const_data();
        auto end = begin + mmap.size();
        size_t counter = 0;
        for (auto iter = begin; iter != end; ++iter) {
            counter += *iter == EOL;
        }
        return counter;
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
    std::atomic<bool> is_stop(false);
    template <int BUFFER_SIZE> struct FileReader {
        char process_buffer[BUFFER_SIZE + 1];

        size_t operator()(const char *datafile) { return read(datafile); }
        size_t read(const char *datafile) {
            char read_buffer[BUFFER_SIZE + 1];
            int fd = ::open(datafile, O_RDONLY | O_NOCTTY);

            // Check that we can open a given file.
            if (fd < 0) {
                std::stringstream writer;
                writer << "Cannot open file \"" << datafile << "\"";
                throw(std::runtime_error(writer.str()));
            }

            // Get file size.
            struct stat buf;
            fstat(fd, &buf);

            // Read data into a read buffer
            size_t block_count = (buf.st_size / BUFFER_SIZE) + (buf.st_size % BUFFER_SIZE != 0);
            size_t counter = 0;
            for (size_t blk = 0; blk < block_count; ++blk) {
                long nbytes = ::read(fd, read_buffer, BUFFER_SIZE);
                if (nbytes < 0) {
                    std::stringstream writer;
                    writer << "Cannot read file \"" << datafile << "\" "
                           << "nbytes = " << nbytes;
                    throw(std::runtime_error(writer.str()));
                };

                memcpy(process_buffer, read_buffer, nbytes);
                counter += process(read_buffer, nbytes);
            }

            // Close our file.
            ::close(fd);

            return counter;
        }

        size_t process(const char *buffer, long nbytes) {
            size_t counter = 0;
            const char *end = buffer + nbytes;
            const char *ptr = buffer;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                ++counter;
                ++ptr;
            }
            // for (auto idx = 0; idx < nbytes; ++idx) {
            //     counter += buffer[idx] == EOL;
            // }
            return counter;
        }
    };
} // namespace

const int number_of_samples = 10;
const int number_of_operations = 2;
const std::string fname("3200.txt");

// Use stat function
// void use_boost(benchmark::State &state) {
//     for (auto _ : state) {
//         benchmark::DoNotOptimize(read_memmap(fname));
//     }
// }

// // Use stat function
// void read_chunk(benchmark::State &state) {
//     for (auto _ : state) {
//         using Reader = FileReader<1 << 16>;
//         FileReader<1 << 16> reader;
//         reader(fname.c_str());
//     }
// }

// // Use stat function
// void read_ioutils(benchmark::State &state) {
//     for (auto _ : state) {
//         using FastLineStats = ioutils::FileReader<ioutils::LineStats_std, 1 << 16>;
//         FastLineStats linestats;
//         linestats(fname.c_str());
//     }
// }

// BENCHMARK(read_chunk);
// BENCHMARK(use_boost);
// BENCHMARK(read_ioutils);
// BENCHMARK_MAIN();

CELERO_MAIN

BASELINE(read, boost_memmap, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(read_memmap(fname.c_str()));
}

BENCHMARK(read, read_chunk, number_of_samples, number_of_operations) {
    FileReader<1 << 16> reader;
    reader(fname.c_str());
}

BENCHMARK(read, ioutils, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<ioutils::LineStats_std, 1 << 16>;
    FastLineStats linestats;
    linestats(fname.c_str());
}

BENCHMARK(read, ioutils_memchr, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<ioutils::LineStats, 1 << 16>;
    FastLineStats linestats;
    linestats(fname.c_str());
}

// int main(const int argc, char *argv[]) {
//     if (argc < 3) {
//         fmt::print("Need to provide an option and a file name\n");
//     }

//     std::string option(argv[1]);
//     std::string fname(argv[2]);
//     if (option == "boost") {
//         fmt::print("Number of lines: {}\n", read_memmap(fname));
//     } else if (option == "chunk") {
//         using Reader = FileReader<1 << 16>;
//         Reader reader;
//         fmt::print("Number of lines: {}\n", reader(fname.c_str()));
//     } else {
//         using FastLineStats = ioutils::FileReader<ioutils::LineStats_std, 1 << 16>;
//         FastLineStats linestats;
//         linestats(fname.c_str());
//         fmt::print("Number of lines: {}\n", linestats.lines);
//     }
// }
