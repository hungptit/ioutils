#include "hs.h"

namespace ioutils {
    auto get_default_regex_mode() -> int { return HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH; }
} // namespace ioutils
