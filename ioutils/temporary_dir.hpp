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

        auto get_path() -> const std::filesystem::path &;

      private:
        using path = std::filesystem::path;
        auto get_unique_string() -> std::string;
        ioutils::RandomStringGenerator gen;
        std::filesystem::path current_dir;
    };
} // namespace ioutils
