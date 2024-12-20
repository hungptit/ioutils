#include "fmt/format.h"
#include "ioutils/stream.hpp"

namespace {
    // A simple policy which prints out the input data.
    struct Policy {
        static void process(const char *buffer, const long nbytes) {
            fmt::print("{}\n", std::string(buffer, buffer + nbytes));
        }
    };

} // namespace

auto main() -> int {
    constexpr size_t BUFLEN = 16;
    using Reader = ioutils::StdinReader<Policy, BUFLEN>;
    Reader reader;
    // reader(STDIN_FILENO);
    return EXIT_SUCCESS;
}
