#pragma once

#include <string>
#include "fmt/format.h"

namespace ioutils {
    namespace experiments {
        struct LineStatsBase {
            LineStatsBase() : lines(0) {}
            void print() const { fmt::print("Number of lines: {}\n", lines); }

          protected:
            size_t lines = 0;
        };

        template <typename Policy> struct LineStats_std : public Policy {
            explicit LineStats_std() : Policy() {}
            void process(const char *buffer, size_t len) {
                for (size_t idx = 0; idx < len; ++idx) {
                    if (buffer[idx] == EOL) {
                        ++Policy::lines;
                    }
                }
            }
        };

        template <typename Policy> struct LineStats : public Policy {
            explicit LineStats() : Policy() {}
            void process(const char *buffer, size_t len) {
                const char *end = buffer + len;
                const char *ptr = buffer;
                while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                    ++Policy::lines;
                    ++ptr;
                }
            }
        };
    } // namespace experiments

} // namespace ioutils
