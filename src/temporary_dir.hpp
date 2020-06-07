#pragma once
#include "utils/random_string.hpp"
#include <filesystem>

namespace ioutils {
    std::string get_unique_string() {
        utils::RandomStringGenerator gen;
        constexpr size_t len = 16;
        return gen(len);
    }

    namespace fs = std::filesystem;
    class TemporaryDirectory {
      public:
        explicit TemporaryDirectory() {
            current_dir = fs::temp_directory_path() / fs::path(get_unique_string());
            fs::create_directories(current_dir);
        }

        explicit TemporaryDirectory(const std::string &parentDir) {
            current_dir = fs::path(parentDir) / fs::path(get_unique_string());
            fs::create_directories(current_dir);
        }

        TemporaryDirectory(const TemporaryDirectory &tmpDir) = delete;
        TemporaryDirectory(TemporaryDirectory &&tmpDir) = default;

        ~TemporaryDirectory() {
            if (fs::exists(current_dir)) {
                fs::remove_all(current_dir);
            }
        }

        const auto &get_path() { return current_dir; }

      private:
        fs::path current_dir;
    };
} // namespace ioutils
