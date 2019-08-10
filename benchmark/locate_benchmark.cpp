#include "celero/Celero.h"

constexpr int number_of_samples = 10;
constexpr int number_of_operations = 5;

int test(const std::string &command, const std::string &regex) {
    std::string buffer = command + regex + " > /tmp/output.log";
    return system(buffer.data());
}

int test_gnu_locate_regex(const std::string &command, const std::string &regex) {
    const std::string dbfile = "locate.db";
    std::string buffer = command + " -d " + dbfile + " --regex " + regex + " > /tmp/locate.log";
    return system(buffer.data());
}

int test_locate_regex(const std::string &command, const std::string &regex) {
    const std::string dbfile = ".database";
    std::string buffer = command + " -d " + dbfile + " " + regex + " > /tmp/fast-locate.log";
    return system(buffer.data());
}

const std::string pattern1{"zstd/.*doc/README[.]md$"};

CELERO_MAIN

#ifdef __APPLE__
std::string locate_command = "glocate";
#else
std::string locate_command = "locate";
#endif

// Find all files in the boost source code
BASELINE(mid, gnu_locate, number_of_samples, number_of_operations) {
    test(locate_command + " -d locate_db_mid --regex ", pattern1);
}

BENCHMARK(mid, fast_locate, number_of_samples, number_of_operations) {
    test("../commands/fast-locate -d .database_mid ", pattern1);
}

// Find all files in the boost source code
BASELINE(big, gnu_locate, number_of_samples, number_of_operations) {
    test(locate_command + " -d locate_db_big --regex ", pattern1);
}

BENCHMARK(big, fast_locate, number_of_samples, number_of_operations) {
    test("../commands/fast-locate -d .database_big ", pattern1);
}
