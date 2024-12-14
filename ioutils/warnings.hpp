#pragma once
namespace ioutils {
    // https://herbsutter.com/2009/10/18/mailbag-shutting-up-compiler-warnings/
    template <class T> void ignore(const T & /*unused*/) {}
} // namespace ioutils
