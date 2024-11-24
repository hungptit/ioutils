#pragma once

#include "ioutils/temporary_dir.hpp"

#include <fstream>
#include <iostream>

namespace ioutils {
    class TestData {
      public:
        using path = std::filesystem::path;
        explicit TestData(const bool verbose = false);
        auto get_path() -> const path &;

      private:
        ioutils::TemporaryDirectory temporary_dir;

        void create_file(const path &aFile, const bool verbose = false);
        void create_symlink(const path &p, const path &symlink, const bool verbose = false);
        auto create_dir(const path &rootFolder, const path &aPath) -> path;

        void init(const bool verbose);
    };
} // namespace ioutils
