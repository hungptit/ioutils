#include "linestats.hpp"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <string>
#include <time.h>

int main(int argc, char *argv[]) {
    using Reader = ioutils::FileReader<ioutils::FileStats, 1 << 16>;
    Reader linestats;
    for (auto idx = 1; idx < argc; ++idx) {
        linestats(argv[idx]);
    }
    linestats.print("Summary:");
}
