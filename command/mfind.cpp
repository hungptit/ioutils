#include "fmt/format.h"
#include "search.hpp"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return EXIT_FAILURE;
    }

	int fd = ::open(argv[1], O_RDONLY);
	if (fd >= 0) {
		std::vector<ioutils::Path> p{{fd, argv[1]}};
		ioutils::FileSearch search;
		search.dfs(p);
	}
	
	return EXIT_SUCCESS;
}
