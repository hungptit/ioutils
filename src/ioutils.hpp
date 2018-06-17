#pragma once
#include "read_policies.hpp"
#include "reader.hpp"
#include <string>

namespace ioutils {
    // Return a string which has the content of a file.
    std::string read(const char *afile) {
        using Reader = typename ioutils::MMapReader<ioutils::AppendPolicy<std::string>>;
        Reader reader;
        reader(afile);
        return reader.data();
    }

} // namespace ioutils
