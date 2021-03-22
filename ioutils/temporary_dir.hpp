#pragma once
#include "ioutils/random_string.hpp"
#include <filesystem>

namespace ioutils {
    namespace fs = std::filesystem;
    class TemporaryDirectory {
      public:
        explicit TemporaryDirectory();
        explicit TemporaryDirectory(const std::string &parentDir);
        TemporaryDirectory(const TemporaryDirectory &tmpDir) = delete;
        TemporaryDirectory(TemporaryDirectory &&tmpDir) = default;
        ~TemporaryDirectory();

        const auto &get_path();

      private:
        std::string get_unique_string();
        ioutils::RandomStringGenerator gen;
        fs::path current_dir;
    };
} // namespace ioutils
