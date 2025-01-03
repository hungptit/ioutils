#pragma once

#include <cstddef>
#include <cstdio>
#include <cstring>
#include "memchr.hpp"

namespace ioutils::experiments {
    static constexpr char EOL = '\n';
    struct LineCoutingAlgorithm {
        LineCoutingAlgorithm() {}
        void print() const { printf("Number of lines: %lu\n", lines); }
        size_t get_count() const {return lines;}

      protected:
        void finalize() const {}
        void set_filename(const char *) {}
        size_t lines = 0;
    };

    template <typename Policy> struct LineStats_std : public Policy {
        LineStats_std() : Policy() {}
        void process(const char *buffer, size_t len, const size_t) {
            for (size_t idx = 0; idx < len; ++idx) {
                if (buffer[idx] == EOL) {
                    ++Policy::lines;
                }
            }
        }
    };

    template <typename Policy> struct LineStats : public Policy {
        LineStats() : Policy() {}
        void process(const char *buffer, size_t len, const size_t) {
            const char *end = buffer + len;
            const char *ptr = buffer;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                ++Policy::lines;
                ++ptr;
            }
        }
        void finalize() const {}
    };
} // namespace ioutils::experiments
