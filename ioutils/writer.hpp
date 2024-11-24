#pragma once
#include "fmt/format.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ioutils {
    auto write(const char *buffer, const size_t buflen, const char *outfile) -> ssize_t {
        int fd = ::open(outfile, O_CREAT | O_WRONLY, S_IRWXU);
        if (fd < 0) {
            fprintf(stderr, "Cannot open file '%s' to write.", outfile);
            return 0;
        }
        auto result = ::write(fd, buffer, buflen);
        close(fd);
        return result;
    }
} // namespace ioutils
