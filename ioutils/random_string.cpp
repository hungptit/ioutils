#include <array>
#include <chrono>
#include <random>
#include <string>

#include "ioutils/random_string.hpp"

namespace ioutils {
    RandomStringGenerator::RandomStringGenerator()
        : rgn((unsigned int)std::chrono::system_clock::now().time_since_epoch().count()) {}

    RandomStringGenerator::RandomStringGenerator(unsigned seed) : rgn(seed) {}

    std::string RandomStringGenerator::operator()(const size_t len) {
        std::string str(len, 0);
        for (size_t idx = 0; idx < len; ++idx) {
            str[idx] = valid_characters[rgn() % N];
        }
        return str;
    }

} // namespace ioutils
