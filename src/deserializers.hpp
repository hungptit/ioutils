#pragma once

#include "cereal/cereal.hpp"
#include "reader.hpp"
#include <sstream>

namespace ioutils {
    template <typename Archive, typename T> struct Cereal {
        void process(const char *begin, const size_t length) {
            std::stringstream ss(std::string(begin, length));
            Archive is(ss);
            is(results);
        }
        T results;              // Stored deserialized results.
    };
} // namespace ioutils
