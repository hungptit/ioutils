#pragma once
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"

namespace ioutils {
    const std::string get_unique_string() {
        return boost::lexical_cast<std::string>(boost::uuids::random_generator()());
    }

    class TemporaryDirectory {
      public:
        explicit TemporaryDirectory() {
            current_dir = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%");
            boost::filesystem::create_directories(current_dir);
        }

        explicit TemporaryDirectory(const std::string &parentDir) {
            current_dir =
                boost::filesystem::path(parentDir) / boost::filesystem::path(get_unique_string());
            boost::filesystem::create_directories(current_dir);
        }

        TemporaryDirectory(const TemporaryDirectory &tmpDir) = delete;

        TemporaryDirectory(TemporaryDirectory &&tmpDir) noexcept
            : current_dir(std::move(tmpDir.current_dir)) {}

        ~TemporaryDirectory() {
            if (boost::filesystem::exists(current_dir)) {
                boost::filesystem::remove_all(current_dir);
            }
        }

        const boost::filesystem::path &get_path() { return current_dir; }

      private:
        boost::filesystem::path current_dir;
    };
}
