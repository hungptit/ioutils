#include "search.hpp"
#include "fmt/format.h"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return EXIT_FAILURE;
    }

	ioutils::filesystem::Error errcode;
	ioutils::filesystem::Utils utils;
    // fmt::print("{}\n", utils.get_absolute_path(argv[1], errcode));
    // fmt::print("{}\n", utils.get_current_directory(errcode));

	{
		// fmt::print("Search for files:\n");
		int fd = ::open(argv[1], O_RDONLY);
		if (fd >= 0) {
			std::vector<ioutils::Path> p {{fd, argv[1]}};
			ioutils::FileSearch search;
			search.dfs(p);		
		}

	}
	
	
	return EXIT_SUCCESS;
}
