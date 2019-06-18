#pragma once

#include "dbconfig.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {
    namespace db {
        constexpr int BUFFER_SIZE = 1 << 16;
        class Reader {
          public:
            Reader(const std::string &keyfile, const std::string &datafile) {}

            bool read(const std::string &key) {
                // Get the offset of a given key.

                // Read the data into buffer until we are done.
                // What do users need to know?
                // 1. Data is read into buffer by chunks.
                // 2. Users have to handle case where their value if bigger than our buffer i.e Policy::process_data
                //    will be called several times.
                // 3. 
                return true;
            }

          private:
            char buffer[BUFFER_SIZE];
            Config config;
            
        };
    } // namespace db
} // namespace ioutils
