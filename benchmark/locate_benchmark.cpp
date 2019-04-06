#include "celero/Celero.h"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <fstream>
#include <iostream>

#include "search.hpp"
#include "search_policies.hpp"
#include "simple_store_policy.hpp"
#include "simple_search_policy.hpp"

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

int test_mlocate_regex(const std::string &command, const std::string &regex) {
    const std::string dbfile = ".database";
    std::string buffer = command + " -d " + dbfile + " " + regex +  " > /tmp/mlocate.log";
    return system(buffer.data());
}

const std::string pattern1{"zstd/.*doc/README[.]md$"};

CELERO_MAIN

// Find all files in the boost source code
BASELINE(regex, gnu_locate, number_of_samples, number_of_operations) {
    test("glocate -d locate.db --regex ", pattern1);
}

BENCHMARK(regex, mlocate, number_of_samples, number_of_operations) {
    test("../commands/mlocate -d .database ", pattern1);
}
