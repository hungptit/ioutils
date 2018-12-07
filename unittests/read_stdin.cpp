#include "fmt/format.h"
#include "stream_reader.hpp"

namespace {
    struct Policy {
        void process(const char *buffer, const long nbytes) {
            fmt::print("Input: {}\n", std::string(buffer, buffer + nbytes));
        }
    };

} // namespace

int main() {
    using Reader = ioutils::StreamReader<Policy>;
    Reader reader;
    reader(STDIN_FILENO);
    return EXIT_SUCCESS;
}
