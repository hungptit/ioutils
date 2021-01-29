#include "clara.hpp"
#include "fmt/format.h"

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

        auto cli = clara::Help(help) | clara::Opt(byte_count, "")["-c"]["--bytes"]("Print the byte counts.") |
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
                       line_count * LINE_COUNT | max_line_length * MAX_LINE_LENGTH | min_line_length * MIN_LINE_LENGTH;

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
