#pragma once
#include <string>
namespace ioutils {
    namespace console {
        static const std::string FOLDER_COLOR = "\033[1;32m";
        static const std::string FILE_COLOR = "\033[1;39m";
        static const std::string SYMLINK_COLOR = "\033[1;31m";
        static const std::string RESET_COLOR = "\033[0m";
    } // namespace console
} // namespace ioutils
