#include "ioutils/temporary_dir.hpp"
#include <filesystem>
#include <string>

namespace ioutils {
    using path = std::filesystem::path;

    auto TemporaryDirectory::get_unique_string() -> std::string {
        constexpr int len = 16;
        return gen(len);
    }

    TemporaryDirectory::TemporaryDirectory() {
        current_dir = std::filesystem::temp_directory_path() / path(get_unique_string());
        create_directories(current_dir);
    }

    TemporaryDirectory::TemporaryDirectory(const std::string &parentDir) {
        current_dir = std::filesystem::path(parentDir) / std::filesystem::path(get_unique_string());
        std::filesystem::create_directories(current_dir);
    }

    TemporaryDirectory::~TemporaryDirectory() {
        if (std::filesystem::exists(current_dir)) {
            std::filesystem::remove_all(current_dir);
        }
    }

    auto TemporaryDirectory::get_path() -> const std::filesystem::path & { return current_dir; }
} // namespace ioutils
