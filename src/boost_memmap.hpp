#pragma once

#include <boost/iostreams/device/mapped_file.hpp>
#include <string>

namespace ioutils {
    // Read a file content into a buffer using memory mapped.
    // Note: This function has a reasonable performance.
    template <typename Container> Container read_memmap(const std::string &afile) {
        boost::iostreams::mapped_file mmap(afile, boost::iostreams::mapped_file::readonly);
        auto begin = mmap.const_data();
        auto end = begin + mmap.size();
        return Container(begin, end);
    }
}
