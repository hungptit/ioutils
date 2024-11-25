#pragma once
#include "read_policies.hpp"
#include "reader.hpp"
#include "search.hpp"
#include <string>

namespace ioutils {
    // Return a string which has the content of a file.
    auto read(const char *afile) -> std::string {
        using Reader = MemoryMappedReader<AppendPolicy<std::string>>;
        Reader reader;
        reader(afile);
        return reader.get_data();
    }
} // namespace ioutils
