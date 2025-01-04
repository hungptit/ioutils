#include "catch2/catch_test_macros.hpp"
#include "experiments.hpp"
#include "fmt/base.h"
#include "ioutils/read_policies.hpp"
#include "ioutils/reader.hpp"
#include "llfio/llfio.hpp"
#include <boost/iostreams/device/mapped_file.hpp>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

namespace {
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
        return {begin, mmap.size()};
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
        size_t counter;
        std::for_each(begin, end, [&counter](auto const item) {
            if (item == EOL) ++counter;
        });
        return counter;
    }

    auto read_all_data_llfio(const std::string &file_path) -> std::string {
        namespace llfio = LLFIO_V2_NAMESPACE;
        auto result = llfio::mapped_file({}, file_path, llfio::file_handle::mode::read,
                                         llfio::file_handle::creation::open_existing,
                                         llfio::file_handle::caching::all, llfio::file_handle::flag::none);
        if (result.has_error()) {
            fmt::print(stderr, "Cannot open: {}\n", file_path);
            return {};
        }
        const auto &mh = result.value();
        const auto len = mh.maximum_extent().value();
        const char *begin = reinterpret_cast<char *>(mh.address());
        const char *end = begin + len;
        return {begin, end};
    }

    auto count_lines_llfio_memchr(const std::string &file_path) -> size_t {
        namespace llfio = LLFIO_V2_NAMESPACE;
        auto result = llfio::mapped_file({}, file_path, llfio::file_handle::mode::read,
                                         llfio::file_handle::creation::open_existing,
                                         llfio::file_handle::caching::all, llfio::file_handle::flag::none);
        if (result.has_error()) {
            fmt::print(stderr, "Cannot open: {}\n", file_path);
            return EXIT_FAILURE;
        }
        const auto &mh = result.value();
        const auto len = mh.maximum_extent().value();
        const char *begin = reinterpret_cast<char *>(mh.address());
        const char *end = begin + len;
        size_t counter = 0;
        while ((begin = static_cast<const char *>(memchr(begin, EOL, end - begin)))) {
            ++counter;
            ++begin;
        }
        return counter;
    }

    auto count_lines_llfio(const std::string &file_path) -> size_t {
        namespace llfio = LLFIO_V2_NAMESPACE;
        auto result = llfio::mapped_file({}, file_path, llfio::file_handle::mode::read,
                                         llfio::file_handle::creation::open_existing,
                                         llfio::file_handle::caching::all, llfio::file_handle::flag::none);
        if (result.has_error()) {
            fmt::print(stderr, "Cannot open: {}\n", file_path);
            return EXIT_FAILURE;
        }
        const auto &mh = result.value();
        const auto len = mh.maximum_extent().value();
        const char *begin = reinterpret_cast<char *>(mh.address());
        const char *end = begin + len;
        size_t counter;
        std::for_each(begin, end, [&counter](auto const item) {
            if (item == EOL) ++counter;
        });
        return counter;
    }

    template <int chunk_size> decltype(auto) read_file_content_ioutils(const std::string &datafile) {
        using Policy = ioutils::AppendPolicy<std::string>;
        ioutils::FileReader<Policy, chunk_size> reader;
        reader(datafile.c_str());
        return reader;
    }

    template <int chunk_size> decltype(auto) count_lines_using_ioutils(const std::string &datafile) {
        using Policy = ioutils::experiments::LineStats<ioutils::experiments::LineCoutingAlgorithm>;
        ioutils::FileReader<Policy, chunk_size> reader;
        reader(datafile.c_str());
        return reader;
    }
} // namespace

namespace {
    constexpr int number_of_samples = 20;
    constexpr int minimum_number_of_operations = 3;
    constexpr int number_of_warmup_runs = 3;
    static const std::string text_data_file("3200.txt");
} // namespace

TEST_CASE("Benchmark different whole file content reading algorithms") {
    SECTION("Validate the correctness of all algorithms") {
        const auto expected_results = read_all_data_std_iostream<std::string>(text_data_file);
        CHECK(read_file_content_ioutils<16>(text_data_file).get_data() == expected_results);
        CHECK(read_all_data_boost_iostreams(text_data_file) == expected_results);
        CHECK(read_all_data_llfio(text_data_file) == expected_results);
    }

    auto bench = ankerl::nanobench::Bench()
                     .warmup(number_of_warmup_runs)
                     .minEpochIterations(minimum_number_of_operations);

    bench.run("iostream", []() {
        ankerl::nanobench::doNotOptimizeAway(read_all_data_std_iostream<std::string>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^10", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 10>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^11", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 11>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^12", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 12>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^13", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 13>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^14", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 14>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^15", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 15>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^16", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 16>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^17", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 17>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^18", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 18>(text_data_file));
    });

    bench.run("file_reader_chunksize=2^19", []() {
        ankerl::nanobench::doNotOptimizeAway(read_file_content_ioutils<1 << 19>(text_data_file));
    });

    bench.run("boost::iostream::mapped_file", []() {
        ankerl::nanobench::doNotOptimizeAway(read_all_data_boost_iostreams(text_data_file));
    });

    bench.run("llfio::mapped_file", []() {
        ankerl::nanobench::doNotOptimizeAway(read_all_data_llfio(text_data_file));
    });
}

TEST_CASE("Benchmark different line counting algorithms") {
    auto bench = ankerl::nanobench::Bench()
                     .warmup(number_of_warmup_runs)
                     .minEpochIterations(minimum_number_of_operations);

    SECTION("Validate the correctness of all algorithms") {
        const size_t expected_result = 302278;
        CHECK(count_lines_using_std_iostream(text_data_file) == expected_result);
        CHECK(count_lines_using_boost_iostreams(text_data_file) == expected_result);
        CHECK(count_lines_llfio(text_data_file) == expected_result);
        CHECK(count_lines_using_ioutils<1 << 16>(text_data_file).get_count() == expected_result);
    }

    SECTION("Benchmark all line counting algorithms") {

        bench.run("Counting lines using std::iostream", []() {
            ankerl::nanobench::doNotOptimizeAway(count_lines_using_std_iostream(text_data_file));
        });

        bench.run("Counting lines using boost::iostreams", []() {
            ankerl::nanobench::doNotOptimizeAway(count_lines_using_boost_iostreams(text_data_file));
        });

        bench.run("Counting lines using llfio",
                  []() { ankerl::nanobench::doNotOptimizeAway(count_lines_llfio(text_data_file)); });

        bench.run("Counting lines using llfio(memchr)",
                  []() { ankerl::nanobench::doNotOptimizeAway(count_lines_llfio_memchr(text_data_file)); });

        bench.run("Counting lines using ioutils",
                  []() { count_lines_using_ioutils<1 << 16>(text_data_file); });
    }
}
