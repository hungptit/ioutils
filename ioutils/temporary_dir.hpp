#pragma once
#include "ioutils/random_string.hpp"
#include <filesystem>

namespace ioutils {
    class TemporaryDirectory {
      public:
        explicit TemporaryDirectory();
        explicit TemporaryDirectory(const std::string &parentDir);
        TemporaryDirectory(const TemporaryDirectory &tmpDir) = delete;
        TemporaryDirectory(TemporaryDirectory &&tmpDir) = default;
        ~TemporaryDirectory();

        const std::filesystem::path &get_path();

      private:
        using path = std::filesystem::path;
        std::string get_unique_string();
        ioutils::RandomStringGenerator gen;
        std::filesystem::path current_dir;
    };
} // namespace ioutils
