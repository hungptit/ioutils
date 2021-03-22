#include "ioutils/temporary_dir.hpp"
#include <filesystem>
#include <string>

namespace ioutils {
    using path = std::filesystem::path;

    std::string TemporaryDirectory::get_unique_string() {
        constexpr int len = 16;
        return gen(len);
    }

    TemporaryDirectory::TemporaryDirectory() : gen() {
        current_dir = std::filesystem::temp_directory_path() / path(get_unique_string());
        create_directories(current_dir);
    }

    TemporaryDirectory::TemporaryDirectory(const std::string &parentDir) : gen() {
        current_dir = std::filesystem::path(parentDir) / std::filesystem::path(get_unique_string());
        std::filesystem::create_directories(current_dir);
    }

    TemporaryDirectory::~TemporaryDirectory() {
        if (std::filesystem::exists(current_dir)) {
            std::filesystem::remove_all(current_dir);
        }
    }

    const std::filesystem::path &TemporaryDirectory::get_path() { return current_dir; }
} // namespace ioutils
