#pragma once
#include "read_policies.hpp"
#include "reader.hpp"
#include "search.hpp"
#include <string>

namespace ioutils {
    // Return a string which has the content of a file.
    std::string read(const char *afile) {
        using Reader = MMapReader<AppendPolicy<std::string>>;
        Reader reader;
        reader(afile);
        return reader.get_data();
    }
} // namespace ioutils
