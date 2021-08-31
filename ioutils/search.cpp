#include "hs.h"

namespace ioutils {
    int get_default_regex_mode() {
        return HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH;
    }
}
