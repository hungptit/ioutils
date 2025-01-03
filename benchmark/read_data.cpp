#include <fstream>
#include <iostream>
// #include "boost_memmap.hpp"
#include "celero/Celero.h"
#include "ioutils/reader.hpp"
#include <iostream>
// #include "memchr.hpp"
// #include "experiments.hpp"
// #include "ioutils/fdreader.hpp"
// #include "ioutils/linestats.hpp"
#include "ioutils/read_policies.hpp"

namespace test {
    constexpr char EOL = '\n';
    template <typename Container> auto read_iostream(const std::string &afile) -> Container {
        std::ifstream t(afile);
        Container str;
        t.seekg(0, std::ios::end);
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        return str;
    }

    auto iostream_linestats(const std::string &afile) -> size_t {
        std::ifstream t(afile);
        size_t lines = 0;
        std::for_each(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>(),
                      [&lines](auto const item) {
                          if (item == EOL) ++lines;
                      });
        return lines;
    }

    // auto memmap_linestats(const std::string &afile) -> size_t {
    //     boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
    //     auto begin = mmap.const_data();
    //     auto end = begin + mmap.size();
    //     size_t lines;
    //     std::for_each(begin, end, [&lines](auto const item) {
    //         if (item == EOL) ++lines;
    //     });
    //     return lines;
    // }
    //

    //

    template <int chunk_size> auto read_file_content_ioutils(const std::string &datafile) {
        using Policy = ioutils::AppendPolicy<std::string>;
        ioutils::FileReader<Policy, chunk_size> reader;
        reader(datafile.c_str());
        return reader.get_data();
    }
} // namespace test

const int number_of_samples = 20;
const int number_of_operations = 2;
const std::string afile("3200.txt");

CELERO_MAIN

BASELINE(read, iostream, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_iostream<std::string>(afile));
}

// BENCHMARK(read, boost_memmap, number_of_samples, number_of_operations) {
//     celero::DoNotOptimizeAway(ioutils::read_memmap<std::string>(afile));
// }

BENCHMARK(read, read_2_10, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 10>(afile));
}

BENCHMARK(read, read_2_11, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 11>(afile));
}

BENCHMARK(read, read_2_12, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 12>(afile));
}

BENCHMARK(read, read_2_13, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 13>(afile));
}

BENCHMARK(read, read_2_14, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 14>(afile));
}

BENCHMARK(read, read_2_15, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 15>(afile));
}

BENCHMARK(read, read_2_16, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 16>(afile));
}

BENCHMARK(read, read_2_17, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 17>(afile));
}

BENCHMARK(read, read_2_18, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 18>(afile));
}

BENCHMARK(read, read_2_19, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 19>(afile));
}

BENCHMARK(read, read_2_20, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::read_file_content_ioutils<1 << 20>(afile));
}

// Read and process data benchmark
BASELINE(linestats, iostream_linestats, number_of_samples, number_of_operations) {
    celero::DoNotOptimizeAway(test::iostream_linestats(afile));
    // std::cout << test::iostream_linestats(afile) << "\n";
}

// BENCHMARK(linestats, memmap_linestats, number_of_samples, number_of_operations) {
//     celero::DoNotOptimizeAway(test::memmap_linestats(afile));
//     // std::cout << test::memmap_linestats(afile) << "\n";
// }

// using LineStatsStd = typename ioutils::experiments::LineStats_std<ioutils::experiments::LineStatsBase>;
// using LineStats = typename ioutils::experiments::LineStats<ioutils::experiments::LineStatsBase>;

// BENCHMARK(linestats, linestats_2_12, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStatsStd, 1 << 12>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, linestats_2_13, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStats, 1 << 13>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, linestats_2_14, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStatsStd, 1 << 14>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, linestats_2_15, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStatsStd, 1 << 15>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, linestats_2_16, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStatsStd, 1 << 16>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, linestats_2_17, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStatsStd, 1 << 17>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, linestats_2_18, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStatsStd, 1 << 18>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, linestats_2_19, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStatsStd, 1 << 19>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, linestats_2_20, number_of_samples, number_of_operations) {
//     using FastLineStats = ioutils::StreamReader<LineStatsStd, 1 << 20>;
//     FastLineStats linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, memchr, number_of_samples, number_of_operations) {
//     using Reader = ioutils::StreamReader<LineStats, 1 << 17>;
//     Reader linestats;
//     linestats(afile.c_str());
// }

// BENCHMARK(linestats, filestats, number_of_samples, number_of_operations) {
//     using Reader = ioutils::StreamReader<ioutils::FileStats, 1 << 17>;
//     Reader linestats;
//     linestats(afile.c_str());
// }
