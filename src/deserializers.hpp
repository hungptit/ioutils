#pragma once

#include "cereal/cereal.hpp"
#include "fmt/format.h"
#include "reader.hpp"
#include <sstream>

namespace ioutils {
    namespace cereal {
        template <typename Archive, typename T> struct ReadPolicy {
            void process(const char *begin, const size_t length) {
                std::stringstream ss(std::string(begin, length));
                Archive is(ss);
                is(results);
            }

            void finalize(){}
            void set_filename(const char*){}
            
            T results; // Stored deserialized results.
        };

        template <typename Archive, typename T> void print(T &data, const std::string &msg) {
            std::stringstream ss;
            {
                Archive archive(ss);
                archive(data);
            }

            if (msg.empty()) {
                fmt::print("{}\n", ss.str());
            } else {
                fmt::print("{0}: {1}\n", msg, ss.str());
            }
        }

    } // namespace cereal
} // namespace ioutils
