#include "linestats.hpp"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <string>
#include <time.h>
#include "boost_memmap.hpp"
#include <iostream>
#include <fstream>

namespace {
    constexpr char EOL = '\n';
    
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
} // namespace

int main(int argc, char *argv[]) {
    using Reader1 = ioutils::FileReader<
        ioutils::experiments::LineStats_std<ioutils::experiments::LineStatsBase>, 1 << 16>;

    using Reader2 = ioutils::FileReader<
        ioutils::experiments::LineStats<ioutils::experiments::LineStatsBase>, 1 << 16>;

    using Reader3 = ioutils::FileReader<ioutils::FileStats, 1 << 16>;

    // Reader2 linestats;
    // for (auto idx = 1; idx < argc; ++idx) {
    //     linestats(argv[idx]);
    // }
    // linestats.print();
    
    for (auto idx = 1; idx < argc; ++idx) {
        fmt::print("Number of lines: {}\n", iostream_linestats(argv[idx]));
        // fmt::print("Number of lines: {}\n", memmap_linestats(argv[idx]));
    }
}
