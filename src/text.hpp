#pragma once

namespace ioutils {
    namespace text {
        enum TextFormat : uint8_t { BOLD, DIM, UNDERLINE, BLINK, REVERSE, HIDDEN };
        enum Color : uint8_t {
            DEFAULT,
            BLACK,
            RED,
            GREEN,
            YELLOW,
            BLUE,
            MAGENTA,
            CYAN,
            LIGHT_GRAY,
            DARK_GRAY,
            LIGHT_RED,
            LIGHT_GREEN,
            LIGHT_YELLOW,
            LIGHT_BLUE,
            LIGHT_MAGENTA,
            LIGHT_CYAN,
            WHITE,
        };

    } // namespace text
} // namespace ioutils
