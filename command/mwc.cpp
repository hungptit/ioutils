#include "clara.hpp"
#include "filesystem.hpp"
#include "fmt/format.h"
#include "linestats.hpp"
#include "reader.hpp"
#include <string>

namespace {
    struct InputParams {
        bool byte_count = false;
        bool char_count = false;
        bool line_count = false;
        bool word_count = false;
        bool max_line_length = false;
        bool min_line_length = false;
        bool use_mmap = false;
        bool verbose = false;
        std::vector<std::string> files;

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(byte_count), CEREAL_NVP(char_count), CEREAL_NVP(line_count),
               CEREAL_NVP(word_count), CEREAL_NVP(max_line_length),
               CEREAL_NVP(min_line_length));
        }
    };

    InputParams parse_input_arguments(int argc, char *argv[]) {
        InputParams params;
        bool help = false;
        auto cli =
            clara::Help(help) |
            clara::Opt(params.byte_count, "")["-c"]["--bytes"]("Print the byte counts.") |
            clara::Opt(params.char_count, "")["-m"]["--chars"]("Print the char counts.") |
            clara::Opt(params.line_count, "")["-l"]["--lines"]("Print the line counts.") |
            clara::Opt(params.word_count, "")["-w"]["--words"]("Print the word counts.") |
            clara::Opt(params.max_line_length,
                       "")["-L"]["--max-line-length"]("Print the maximum line width.") |
            clara::Opt(params.min_line_length,
                       "")["-M"]["--min-line-length"]("Print the minimum line width.") |
            clara::Opt(params.verbose)["-v"]["--verbose"]("Display verbose information") |
            clara::Opt(params.use_mmap)["--use-mmap"]("Read by mapping file into memory.") |
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

        // Display input arguments in JSON format if verbose flag is on
        if (params.verbose) {
            std::stringstream ss;
            {
                cereal::JSONOutputArchive ar(ss);
                ar(cereal::make_nvp("Input arguments", params));
            }
            fmt::print("{}\n", ss.str());
        }

        return params;
    }

    template <typename Reader, typename Params> void wc(Params &&params) {
        Reader linestats;
        for (auto const &afile : params.files) {
            linestats(afile.data());
        }
        linestats.print();
    }

} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    if (params.use_mmap) {
        using Reader = ioutils::MMapReader<ioutils::FileStats>;
        wc<Reader>(params);
    } else {
        constexpr int BUFFER_SIZE = 1 << 16;
        using Reader = ioutils::FileReader<ioutils::FileStats, BUFFER_SIZE>;
        wc<Reader>(params);
    }
    return EXIT_SUCCESS;
}
