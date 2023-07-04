#include "fmt/format.h"
#include <algorithm>
#include <argparse/argparse.hpp>

#include "ioutils/filesystem.hpp"
#include "ioutils/linestats.hpp"
#include "ioutils/reader.hpp"
#include <string>

namespace {
    enum PARAMS {
        VERBOSE = 1,
        MMAP = 1 << 1,
        BYTE_COUNT = 1 << 2,
        CHAR_COUNT = 1 << 3,
        LINE_COUNT = 1 << 4,
        WORD_COUNT = 1 << 5,
        MAX_LINE_LENGTH = 1 << 6,
        MIN_LINE_LENGTH = 1 << 7,
    };

    struct InputParams {
        int flags;

        bool verbose() const { return (flags & VERBOSE) > 0; }
        bool mmap() const { return (flags & MMAP) > 0; }
        bool byte_count() const { return (flags & BYTE_COUNT) > 0; }
        bool char_count() const { return (flags & CHAR_COUNT) > 0; }
        bool line_count() const { return (flags & LINE_COUNT) > 0; }
        bool word_count() const { return (flags & WORD_COUNT) > 0; }
        bool max_line_length() const { return (flags & MAX_LINE_LENGTH) > 0; }
        bool min_line_length() const { return (flags & MIN_LINE_LENGTH) > 0; }

        std::vector<std::string> files;

        void print() const {
            fmt::print("verbose: {}\n", verbose());
            fmt::print("mmap: {}\n", mmap());
            fmt::print("byte_count: {}\n", byte_count());
            fmt::print("char_count: {}\n", char_count());
            fmt::print("line_count: {}\n", line_count());
            fmt::print("word_count: {}\n", word_count());
            fmt::print("max_line_length: {}\n", max_line_length());
            fmt::print("min_line_length: {}\n", min_line_length());
        }
    };

    InputParams parse_input_arguments(int argc, char *argv[]) {
        InputParams params;
        bool help = false;
        bool verbose = 0;
        bool mmap = false;
        bool byte_count = true;
        bool char_count = false;
        bool word_count = false;
        bool line_count = false;
        bool max_line_length = false;
        bool min_line_length = false;

        argparse::ArgumentParser parser("fast-wc");

        parser.add_argument("--path", "-p").required().help("The input file or folder").nargs(1);

        parser.add_argument("--verbose", "-v")
            .help("Display the verbose information")
            .default_value(false)
            .implicit_value(true)
            .action([&](const auto &) { verbose = true; });
        parser.add_argument("--chars", "-c")
            .help("Print the char count")
            .default_value(true)
            .implicit_value(true)
            .action([&](const auto &) { line_count = true; });

        parser.add_argument("--lines", "-l")
            .help("Print the line count")
            .default_value(true)
            .implicit_value(true)
            .action([&](const auto &) { line_count = true; });

        parser.add_argument("--words", "-w")
            .help("Print the word count")
            .default_value(true)
            .implicit_value(true)
            .action([&](const auto &) { word_count = true; });

        parser.add_argument("--max-line-length")
            .help("Print the maximum line length")
            .default_value(false)
            .implicit_value(true)
            .action([&](const auto &) { max_line_length = true; });

        parser.add_argument("--min-line-length")
            .help("Print the minimum line length")
            .default_value(false)
            .implicit_value(true)
            .action([&](const auto &) { min_line_length = true; });

        parser.add_argument("--mmap")
            .help("Read the file contents using mapped memory.")
            .default_value(false)
            .implicit_value(true)
            .action([&](const auto &) { mmap = true; });

        try {
            parser.parse_args(argc, argv);
        } catch (const std::runtime_error &err) {
            std::cerr << err.what() << std::endl;
            std::cerr << parser;
            throw;
        }

        // Update flags
        params.flags = verbose * VERBOSE | mmap * MMAP | byte_count * BYTE_COUNT | word_count * WORD_COUNT |
                       line_count * LINE_COUNT | max_line_length * MAX_LINE_LENGTH | min_line_length * MIN_LINE_LENGTH;

        if (params.verbose()) params.print();

        params.files = parser.get<std::vector<std::string>>("--path");

        return params;
    }

    template <typename Reader, typename Params> void wc(Params &&params) {
        Reader linestats(params);
        for (auto const &afile : params.files) {
            linestats(afile.data());
        }
        linestats.print();
    }

} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    if (params.mmap()) {
        using Reader = ioutils::MemoryMappedReader<ioutils::FileStats>;
        wc<Reader>(params);
    } else {
        constexpr int BUFFER_SIZE = 1 << 16;
        using Reader = ioutils::FileReader<ioutils::FileStats, BUFFER_SIZE>;
        wc<Reader>(params);
    }
    return EXIT_SUCCESS;
}
