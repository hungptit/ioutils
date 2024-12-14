#pragma once

#include <boost/iostreams/device/mapped_file.hpp>
#include <string>

namespace ioutils {
    // Read a file content into a buffer using memory mapped.
    // Note: This function has a reasonable performance.
    template <typename Container> auto read_memmap(const std::string &afile) -> Container {
        boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
        const auto *begin = mmap.const_data();
        const auto *end = begin + mmap.size();
        return Container(begin, end);
    }
} // namespace ioutils
