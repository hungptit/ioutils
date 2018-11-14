#pragma once

#include <cstdint>

namespace ioutils {
    constexpr char SEP = '/';
    enum DisplayType : uint8_t {
        DISP_NONE = 0,
        DISP_FILE = 1,
        DISP_DIR = 2,
        DISP_SYMLINK = 1 << 2,
        DISP_EXECUTABLE = 1 << 3,
        DISP_COLOR = 1 << 4,
        FOLLOW_SYMBOLIC_LINKS = 1 << 5,
        MATCH_FULL_PATH = 1 << 6,
    };
} // namespace ioutils
