#include "linestats.hpp"
#include "../benchmark/experiments.hpp"
#include "boost_memmap.hpp"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

// namespace experiments {
//     constexpr char EOL = '\n';

//     size_t iostream_linestats(const std::string &afile) {
//         std::ifstream t(afile);
//         size_t lines = 0;
//         std::for_each(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>(),
//                       [&lines](auto const item) {
//                           if (item == EOL) ++lines;
//                       });
//         return lines;
//     }

//     size_t memmap_linestats(const std::string &afile) {
//         boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
//         auto begin = mmap.const_data();
//         auto end = begin + mmap.size();
//         size_t lines;
//         std::for_each(begin, end, [&lines](auto const item) {
//             if (item == EOL) ++lines;
//         });
//         return lines;
//     }
// } // namespace

int main(int argc, char *argv[]) {
    constexpr int BUFFER_SIZE = 1 << 17;
    // using Reader1 = ioutils::FileReader<
    //     ioutils::experiments::LineStats_std<ioutils::experiments::LineStatsBase>, BUFFER_SIZE>;

    // using Reader2 = ioutils::FileReader<
    //     ioutils::experiments::LineStats<ioutils::experiments::LineStatsBase>, BUFFER_SIZE>;

    using Reader3 = ioutils::FileReader<ioutils::FileStats, BUFFER_SIZE>;
    // using Reader4 = ioutils::MMapReader<ioutils::FileStats>;

    Reader3 linestats;
    for (auto idx = 1; idx < argc; ++idx) {
        linestats(argv[idx]);
    }
    linestats.print();

    // for (auto idx = 1; idx < argc; ++idx) {
    //     fmt::print("Number of lines: {}\n", iostream_linestats(argv[idx]));
    //     // fmt::print("Number of lines: {}\n", memmap_linestats(argv[idx]));
    // }
}
