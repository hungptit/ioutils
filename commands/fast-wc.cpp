#include "clara.hpp"
#include "fmt/base.h"
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

        [[nodiscard]] auto verbose() const -> bool { return (flags & VERBOSE) > 0; }
        [[nodiscard]] auto mmap() const -> bool { return (flags & MMAP) > 0; }
        [[nodiscard]] auto byte_count() const -> bool { return (flags & BYTE_COUNT) > 0; }
        [[nodiscard]] auto char_count() const -> bool { return (flags & CHAR_COUNT) > 0; }
        [[nodiscard]] auto line_count() const -> bool { return (flags & LINE_COUNT) > 0; }
        [[nodiscard]] auto word_count() const -> bool { return (flags & WORD_COUNT) > 0; }
        [[nodiscard]] auto max_line_length() const -> bool { return (flags & MAX_LINE_LENGTH) > 0; }
        [[nodiscard]] auto min_line_length() const -> bool { return (flags & MIN_LINE_LENGTH) > 0; }

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

    auto parse_input_arguments(int argc, char *argv[]) -> InputParams {
        InputParams params;
        bool help = false;
        bool verbose = false;
        bool mmap = false;
        bool byte_count = true;
        bool char_count = false;
        bool word_count = false;
        bool line_count = false;
        bool max_line_length = false;
        bool min_line_length = false;

        auto cli =
            clara::Help(help) | clara::Opt(byte_count, "")["-c"]["--bytes"]("Print the byte counts.") |
            clara::Opt(char_count, "")["-m"]["--chars"]("Print the char counts.") |
            clara::Opt(line_count, "")["-l"]["--lines"]("Print the line counts.") |
            clara::Opt(word_count, "")["-w"]["--words"]("Print the word counts.") |
            clara::Opt(max_line_length, "")["-L"]["--max-line-length"]("Print the maximum line width.") |
            clara::Opt(min_line_length, "")["-M"]["--min-line-length"]("Print the minimum line width.") |
            clara::Opt(verbose)["-v"]["--verbose"]("Display verbose information") |
            clara::Opt(mmap)["--mmap"]("Read by mapping file into memory.") |
            clara::Arg(params.files, "files")("Input files or folders");

        auto result = cli.parse(clara::Args(argc, argv));
        if (!result) {
            fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
            exit(EXIT_FAILURE);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            fmt::print("{}", oss.str());
            exit(EXIT_SUCCESS);
        }

        // Update flags
        params.flags = verbose * VERBOSE | mmap * MMAP | byte_count * BYTE_COUNT | word_count * WORD_COUNT |
                       line_count * LINE_COUNT | max_line_length * MAX_LINE_LENGTH |
                       min_line_length * MIN_LINE_LENGTH;

        // Display input arguments in JSON format if verbose flag is on
        if (params.verbose()) params.print();

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

auto main(int argc, char *argv[]) -> int {
    const auto params = parse_input_arguments(argc, argv);
    if (params.mmap()) {
        wc<ioutils::MemoryMappedReader<ioutils::FileStats>>(params);
    } else {
        constexpr int BUFFER_SIZE = 1 << 16;
        wc<ioutils::FileReader<ioutils::FileStats, BUFFER_SIZE>>(params);
    }
    return EXIT_SUCCESS;
}
