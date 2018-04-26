#include <fstream>
#include <iostream>

#include "fmt/format.h"

#include "boost_memmap.hpp"
#include "celero/Celero.h"
#include "ioutils.hpp"
#include <iostream>

#include "utils/memchr.hpp"

namespace test {
    constexpr char EOL = '\n';
    template <typename Container> Container read_iostream(const std::string &afile) {
        std::ifstream t(afile);
        Container str;
        t.seekg(0, std::ios::end);
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        return str;
    }

    size_t iostream_linestats(const std::string &afile) {
        std::ifstream t(afile);
        size_t lines = 0;
        std::for_each(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>(),
                      [&lines](auto const item) {
                          if (item == EOL) ++lines;
                      });
        return lines;
    }

    size_t memmap_linestats(const std::string &afile) {
        boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
        auto begin = mmap.const_data();
        auto end = begin + mmap.size();
        size_t lines;
        std::for_each(begin, end, [&lines](auto const item) {
            if (item == EOL) ++lines;
        });
        return lines;
    }

    struct LineStats {
        explicit LineStats() : lines(0) {}
        void process(const char *buffer, size_t len) {
            for (size_t idx = 0; idx < len; ++idx) {
                if (buffer[idx] == EOL) {
                    ++lines;
                }
            }
        }
        size_t lines;
    };

    struct LineStats_memchr {
        explicit LineStats_memchr() : lines(0) {}
        void process(const char *buffer, size_t len) {
            const char *end = buffer + len;
            const char *ptr = buffer;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                ++lines;
                ++ptr;
            }
        }
        // Store the number of lines.
        size_t lines;
    };
} // namespace test

test::LineStats policy;

const int number_of_samples = 10;
const int number_of_operations = 2;
const std::string afile("3200.txt");

CELERO_MAIN

BASELINE(read, iostream, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_iostream<std::string>(afile));
}

BENCHMARK(read, boost_memmap, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(ioutils::read_memmap<std::string>(afile));
}

BENCHMARK(read, read_2_10, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 10>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_11, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 11>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_12, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 12>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_13, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 13>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_14, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 14>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_15, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 15>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_16, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 16>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_17, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 17>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_18, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 18>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_19, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 19>;
    Reader reader;
    reader(afile.c_str());	
}

BENCHMARK(read, read_2_20, number_of_samples, number_of_operations) {
	using Reader = ioutils::FileReader<ioutils::AppendPolicy, 1 << 20>;
    Reader reader;
    reader(afile.c_str());	
}


// Read and process data benchmark
BASELINE(linestats, iostream_linestats, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::iostream_linestats(afile));
    // std::cout << test::iostream_linestats(afile) << "\n";
}

BENCHMARK(linestats, memmap_linestats, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::memmap_linestats(afile));
    // std::cout << test::memmap_linestats(afile) << "\n";
}

BENCHMARK(linestats, linestats_2_12, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 12>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, linestats_2_13, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 13>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, linestats_2_14, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 14>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, linestats_2_15, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 15>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, linestats_2_16, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 16>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, linestats_2_17, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 17>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, linestats_2_18, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 18>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, linestats_2_19, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 19>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, linestats_2_20, number_of_samples, number_of_operations) {
    using FastLineStats = ioutils::FileReader<test::LineStats, 1 << 20>;
    FastLineStats linestats;
    linestats(afile.c_str());
}

BENCHMARK(linestats, memchr, number_of_samples, number_of_operations) {
    using Reader = ioutils::FileReader<test::LineStats_memchr, 1 << 16>;
    Reader linestats;
    linestats(afile.c_str());
}
