#pragma once
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "fmt/format.h"

namespace ioutils {
    ssize_t write(const char *buffer, const size_t buflen, const char *outfile) {
		int fd = ::open(outfile, O_CREAT | O_WRONLY);
		if (fd < 0) {
			fmt::print(stderr, "Cannot open file '{}' to write.", outfile);
			return 0;
		}
		auto result = ::write(fd, buffer, buflen);
		close(fd);
		return result;
	}
	
} // namespace ioutils
