#include <fstream>
#include <iostream>

#include "fmt/format.h"

#include "boost_memmap.hpp"
#include "celero/Celero.h"
#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/xml.hpp"
#include "ioutils.hpp"

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

    size_t iostream_linstats(const std::string &afile) {
        std::ifstream t(afile);
        size_t lines = 0;
        // str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
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
        void operator()(const char *buffer, size_t len) {
            for (auto idx = 0; idx < len; ++idx) {
                if (buffer[idx] == EOL) {
                    ++lines;
                }
            }
        }
        size_t lines = 0;
    };

} // namespace test

const int number_of_samples = 10;
const int number_of_iterator = 100;
const std::string afile("/Users/hdang/working/scribe_parser/data/workqueue-execution_current");

CELERO_MAIN

BASELINE(read, iostream, number_of_samples, number_of_iterator) {
    celero::DoNotOptimizeAway(test::read_iostream<std::string>(afile));
}

BENCHMARK(read, boost_memmap, number_of_samples, number_of_iterator) {
    celero::DoNotOptimizeAway(ioutils::read_memmap<std::string>(afile));
}

BENCHMARK(read, read_2_10, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1024>(afile.c_str(), data);
}

BENCHMARK(read, read_2_12, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 4096>(afile.c_str(), data);
}

BENCHMARK(read, read_2_13, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1 << 13>(afile.c_str(), data);
}

BENCHMARK(read, read_2_14, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1 << 14>(afile.c_str(), data);
}

BENCHMARK(read, read_2_15, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1 << 15>(afile.c_str(), data);
}

BENCHMARK(read, read_2_16, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1 << 16>(afile.c_str(), data);
}

BENCHMARK(read, read_2_17, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1 << 17>(afile.c_str(), data);
}

BENCHMARK(read, read_2_18, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1 << 18>(afile.c_str(), data);
}

BENCHMARK(read, read_2_19, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1 << 19>(afile.c_str(), data);
}

BENCHMARK(read, read_2_20, number_of_samples, number_of_iterator) {
    std::string data;
    ioutils::read<std::string, 1048576>(afile.c_str(), data);
}
