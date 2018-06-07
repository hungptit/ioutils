#pragma once
#include "read_policies.hpp"
#include "reader.hpp"
#include <string>

namespace ioutils {
    // Return a string which has the content of a file.
    template <size_t BUFFER_SIZE = READ_TRUNK_SIZE> std::string read(const char *afile) {
        static_assert(READ_TRUNK_SIZE > (1 << 13),
                      "READ_TRUNK_SIZE should be greater than 4K!");
        using Reader = FileReader<AppendPolicy<std::string>, BUFFER_SIZE>;
        Reader reader;
        reader(afile);
        return reader.data();
    }

} // namespace ioutils
