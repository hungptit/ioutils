#pragma once

#include <cstdint>
namespace ioutils {
    enum FileSearchFlags : std::uint32_t {
        VERBOSE = 1,                // Display verbose information.
        INVERT_MATCH = 1 << 1,      // Display paths that do not match given pattern.
        COLOR = 1 << 2,             // Display color text.
        IGNORE_FILE = 1 << 3,       // Skip files.
        IGNORE_DIR = 1 << 4,        // Skip directories.
        IGNORE_SYMLINK = 1 << 5,    // Skip symlinks.
        FOLLOW_SYMLINK = 1 << 6,    // Dive into to the symlink.
        DFS = 1 << 7,               // Use DFS for file traversal.
        IGNORE_FIFO = 1 << 8,       // Ignore pipe
        IGNORE_CHR = 1 << 9,        // Ignore character special
        IGNORE_BLK = 1 << 10,       // Ignore block special
        IGNORE_SOCKET = 1 << 11,    // Ignore sockets
        IGNORE_WHITEOUT = 1 << 12,  // Ignore whiteout files
        IGNORE_UNKNOWN = 1 << 13,   // Ignore unknown paths
        DONOT_IGNORE_GIT = 1 << 14, // Ignore git folders.
        IGNORE_ERROR = 1 << 15,     // Do not print out errors.
    };
}
