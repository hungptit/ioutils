#pragma once
#include <string>

namespace ioutils {
    void remove_trailing_slash(std::string &s) {
        constexpr char SLASH = '/';
        while (!s.empty()) {
            if (s.back() == SLASH) {
                s.pop_back();
            } else {
                break;
            }
        }
    }
} // namespace ioutils
