#include "catch2/catch_test_macros.hpp"
#include "celero/Celero.h"
#include "ioutils/reader.hpp"
#include <boost/iostreams/device/mapped_file.hpp>
#include <fstream>
#include <iostream>
#include <string>
// #include "memchr.hpp"
// #include "experiments.hpp"
// #include "ioutils/fdreader.hpp"
// #include "ioutils/linestats.hpp"
#include "ioutils/read_policies.hpp"

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

namespace test {
    constexpr char EOL = '\n';
    template <typename Container> auto read_all_data_std_iostream(const std::string &afile) -> Container {
        std::ifstream t(afile);
        Container str;
        t.seekg(0, std::ios::end);
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        return str;
    }

    auto read_all_data_boost_iostreams(const std::string &afile) -> std::string {
        boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
        auto begin = mmap.const_data();
        return std::string(begin, mmap.size());
    }

    auto count_lines_using_std_iostream(const std::string &afile) -> size_t {
        std::ifstream t(afile);
        size_t lines = 0;
        std::for_each(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>(),
                      [&lines](auto const item) {
                          if (item == EOL) ++lines;
                      });
        return lines;
    }

    auto count_lines_using_boost_iostreams(const std::string &afile) -> size_t {
        boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
        auto begin = mmap.const_data();
        auto end = begin + mmap.size();
        size_t lines;
        std::for_each(begin, end, [&lines](auto const item) {
            if (item == EOL) ++lines;
        });
        return lines;
    }

    template <int chunk_size> decltype(auto) read_file_content_ioutils(const std::string &datafile) {
        using Policy = ioutils::AppendPolicy<std::string>;
        ioutils::FileReader<Policy, chunk_size> reader;
        reader(datafile.c_str());
        return reader;
    }
} // namespace test

namespace {
    constexpr int number_of_samples = 20;
    constexpr int minimum_number_of_operations = 3;
    constexpr int number_of_warmup_runs = 3;
    static const std::string text_data_file("3200.txt");
} // namespace

TEST_CASE("Benchmark different whole file content reading algorithms") {
    SECTION("Validate the correctness of all algorithms") {
        const auto expected_results = test::read_all_data_std_iostream<std::string>(text_data_file);
        CHECK(test::read_file_content_ioutils<16>(text_data_file).get_data() == expected_results);
        CHECK(test::read_all_data_boost_iostreams(text_data_file) == expected_results);
    }

    auto bench = ankerl::nanobench::Bench()
                     .warmup(number_of_warmup_runs)
                     .minEpochIterations(minimum_number_of_operations);

    bench.run("iostream", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_all_data_std_iostream<std::string>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^10", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 10>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^11", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 11>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^12", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 12>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^13", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 13>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^14", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 14>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^15", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 15>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^16", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 16>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^17", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 17>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^18", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 18>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^19", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_file_content_ioutils<1 << 19>(text_data_file));
    });

    bench.run("boost::iostream::mapped_file", []() {
        ankerl::nanobench::doNotOptimizeAway(test::read_all_data_boost_iostreams(text_data_file));
    });
}

TEST_CASE("Benchmark different line counting algorithms") {
    auto bench = ankerl::nanobench::Bench()
                     .warmup(number_of_warmup_runs)
                     .minEpochIterations(minimum_number_of_operations);

    SECTION("Validate the correctness of all algorithms") {}

    SECTION("Benchmark all line counting algorithms") {

        bench.run("Counting lines using std::iostream",
                  []() { ankerl::nanobench::doNotOptimizeAway(test::count_lines_using_std_iostream(text_data_file)); });

        bench.run("Counting lines using boost::iostreams",
                  []() { ankerl::nanobench::doNotOptimizeAway(test::count_lines_using_boost_iostreams(text_data_file)); });
    }
}

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
