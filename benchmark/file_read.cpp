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
#include "read_policies.hpp"
#include "reader.hpp"
#include <deque>

#include "celero/Celero.h"
#include "experiments.hpp"

namespace {
    constexpr char EOL('\n');
    // Read file using memory mapped approach.
    size_t read_memmap(const char *afile) {
        boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
        auto begin = mmap.const_data();
        auto end = begin + mmap.size();
        size_t counter = 0;
        for (auto iter = begin; iter != end; ++iter) {
            counter += *iter == EOL;
        }
        return counter;
    }

    // Low level memmap
    size_t read_mmap(const char *afile) {
        size_t counter = 0;
        int fd = open(afile, O_RDONLY);

        // Error handling object.
        auto handle_error = [afile](const std::string &errmsg) {
            const std::string msg = std::string("Cannot open ") + afile;
            throw(msg);
        };

        if (fd == -1) {
            handle_error("Cannot open ");
        }

        // Obtain file size
        struct stat info;
        if (fstat(fd, &info) == -1) {
            handle_error("Cannot get information of ");
        }
        size_t length = info.st_size;

        char *begin =
            static_cast<char *>(mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0u));

        if (begin == MAP_FAILED) {
            handle_error("Cannot map ");
        }

        // Tell the kernel that we will access file content sequentially.
        if (madvise(begin, length, MADV_SEQUENTIAL | MADV_WILLNEED) != 0) {
            write(1, "Cannot set hint for kernel!", 27);
        }

        // Count the number of line
        const char *end = begin + length;
        const char *ptr = begin;
        while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
            ++counter;
            ++ptr;
        }

        // Cleanup
        munmap(begin, length);
        close(fd);

        return counter;
    }

    // Read from file in chunks
    struct AppendPolicy {
        void process(const char *buffer, const size_t len) { data.append(buffer, len); }
        void finalize(){}       // Do not need to do anything for the append only policy.
        std::string data;
    };

    struct ProducerPolicy {
        void process(const char *buffer, const size_t len) {
            data.emplace_back(std::string(buffer, len));
        }
        std::deque<std::string> data;
    };

    constexpr size_t READ_BUFFER_SIZE = 1 << 17;

    // A reader class which stores the policy as a member data.
    template <int BUFFER_SIZE = READ_BUFFER_SIZE> struct FileReader {
        char process_buffer[BUFFER_SIZE];

        size_t operator()(const char *datafile) { return read(datafile); }
        size_t read(const char *datafile) {
            char read_buffer[BUFFER_SIZE];
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
            size_t counter = 0;
            std::string linebuf;
            for (size_t blk = 0; blk < block_count; ++blk) {
                long nbytes = ::read(fd, read_buffer, BUFFER_SIZE);
                if (nbytes < 0) {
                    const std::string msg =
                        std::string("Cannot read from file \"") + std::string(datafile) + "\" ";
                    throw(std::runtime_error(msg));
                };

                // Update counter
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

            // Use memchr
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                ++counter;
                ++ptr;
            }

            // Simple for-loop
            // for (auto idx = 0; idx < nbytes; ++idx) {
            //     counter += buffer[idx] == EOL;
            // }
            return counter;
        }
    };
} // namespace

const int number_of_samples = 40;
const int number_of_operations = 2;
const std::string fname("3200.txt");

// CELERO_MAIN

// BASELINE(read, boost_memmap, number_of_samples, number_of_operations) {
//     celero::DoNotOptimizeAway(read_memmap(fname.c_str()));
// }

// BENCHMARK(read, mmap_reader_memchr, number_of_samples, number_of_operations) {
//     using Policy = ioutils::experiments::LineStats<ioutils::experiments::LineStatsBase>;
//     using LineStats = ioutils::MMapReader<Policy>;
//     LineStats linestats;
//     linestats(fname.c_str());
// }

// BENCHMARK(read, memmap, number_of_samples, number_of_operations) {
//     celero::DoNotOptimizeAway(read_mmap(fname.c_str()));
// }

// BENCHMARK(read, read_chunk, number_of_samples, number_of_operations) {
//     FileReader<READ_BUFFER_SIZE> reader;
//     reader(fname.c_str());
// }

// BENCHMARK(read, ioutils_std, number_of_samples, number_of_operations) {
//     using Policy = ioutils::experiments::LineStats_std<ioutils::experiments::LineStatsBase>;
//     using LineStats = ioutils::FileReader<Policy, READ_BUFFER_SIZE>;
//     LineStats linestats;
//     linestats(fname.c_str());
// }

// BENCHMARK(read, ioutils_memchr, number_of_samples, number_of_operations) {
//     using Policy = ioutils::experiments::LineStats<ioutils::experiments::LineStatsBase>;
//     using LineStats = ioutils::FileReader<Policy, READ_BUFFER_SIZE>;
//     LineStats linestats;
//     linestats(fname.c_str());
// }

int main(const int argc, char *argv[]) {
    if (argc < 3) {
        fmt::print("Need to provide an option and a file name\n");
    }

    std::string option(argv[1]);
    if (option == "boost") {
        fmt::print("Number of lines: {}\n", read_memmap(argv[2]));
    } else if (option == "mmap") {
        using Policy = ioutils::experiments::LineStats<ioutils::experiments::LineStatsBase>;
        using LineStats = ioutils::MMapReader<Policy>;
        LineStats linestats;
        linestats(argv[2]);
        linestats.print();
    } else if (option == "chunk") {
        using Reader = FileReader<1 << 16>;
        Reader reader;
        fmt::print("Number of lines: {}\n", reader(argv[2]));
    } else {
        using Policy = ioutils::experiments::LineStats<ioutils::experiments::LineStatsBase>;
        using LineStats = ioutils::FileReader<Policy, READ_BUFFER_SIZE>;
        LineStats linestats;
        linestats(argv[2]);
        linestats.print();
    }
}
