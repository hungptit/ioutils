#include "search.hpp"
#include "fmt/format.h"
#include "utils/timeutils.hpp"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return EXIT_FAILURE;
    }

	ioutils::filesystem::Error errcode;
	ioutils::filesystem::Utils utils;
    fmt::print("{}\n", utils.get_absolute_path(argv[1], errcode));
    fmt::print("{}\n", utils.get_current_directory(errcode));

	int fd = ::open(argv[1], O_RDONLY);
	if (fd < 0) return EXIT_FAILURE;
	struct stat props;
    int retval = fstat(fd, &props);
	
	utils::TimePrinter printer("%Y-%m-%d %H:%M:%S");
	
	fmt::print("st_mode: {0:o}\n", props.st_mode);

	if (ioutils::filesystem::is_directory(props.st_mode)) {
		fmt::print("Directory size: {0}\n", props.st_size);
		auto dirp = fdopendir(fd);
		if (dirp != nullptr) {
			struct dirent *info;
			while ((info = readdir(dirp)) != NULL) {
				fmt::print("len: {0}, name: {1}\n", info->d_reclen, info->d_name);
			}
			(void)closedir(dirp);
		}
	} else if (ioutils::filesystem::is_regular_file(props.st_mode)) {
		fmt::print("File size: {0}\n", props.st_size);
	} else if (ioutils::filesystem::is_symlink(props.st_mode)) {
		
	} else {
		fmt::print("How can we get here?\n");
	}
	::close(fd);
	fmt::print("Access time: {0}\n", printer(props.st_atime));
	fmt::print("Modified time: {0}\n", printer(props.st_mtime));
	fmt::print("Status change time: {0}\n", printer(props.st_ctime));	
	
	return EXIT_SUCCESS;
}
