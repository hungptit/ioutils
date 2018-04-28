#include "search.hpp"
#include "fmt/format.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return EXIT_FAILURE;
    }
    const char *path =argv[1];
    fmt::print("{}\n", path);

    auto iter = fstat();

    return EXIT_SUCCESS;
}
