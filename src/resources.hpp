#pragma once

namespace ioutils {
    constexpr char SEP = '/';
    enum DisplayType : uint8_t {
        DISP_NONE = 0,
        DISP_FILE = 1,
        DISP_DIR = 2,
        DISP_SYMLINK = 4,
        DISP_EXECUTABLE = 8
    };
} // namespace ioutils
