#include "test_data.hpp"
#include "ioutils/temporary_dir.hpp"

#include <fstream>
#include <iostream>

namespace ioutils {
    using path = std::filesystem::path;

    TestData::TestData(const bool verbose) : temporary_dir() { init(verbose); }
    auto TestData::get_path() -> const path & { return temporary_dir.get_path(); }

    void TestData::create_file(const path &aFile, const bool verbose) {
        std::ofstream outfile(aFile.string());
        outfile << "Fake data for testing.\n";
        outfile.close();
        if (std::filesystem::exists(aFile)) {
            if (verbose) std::cout << aFile.string() << " is created" << std::endl;
        } else {
            std::cerr << "Cannot create " << aFile.string() << std::endl;
        }
    }

    void TestData::create_symlink(const path &p, const path &symlink, const bool verbose) {
        std::filesystem::create_symlink(p, symlink);
        if (verbose) {
            if (verbose) std::cout << symlink.string() << " is linked" << std::endl;
        }
    }

    auto TestData::create_dir(const path &rootFolder, const path &aPath) -> path {
        auto fullPath = rootFolder / aPath;
        std::filesystem::create_directories(fullPath);
        return fullPath;
    }

    void TestData::init(const bool verbose) {
        create_file(temporary_dir.get_path() / path("README.md"));

        auto dataFolder = create_dir(temporary_dir.get_path(), "data");
        create_file(dataFolder / path("data.mat"));

        auto gitFolder = create_dir(temporary_dir.get_path(), ".git");
        create_file(gitFolder / path("test.cpp"));

        auto subversion_folder = create_dir(temporary_dir.get_path(), ".subversion");
        create_file(subversion_folder / path("foo.p"), verbose);

        create_dir(temporary_dir.get_path(), "CMakeFiles");

        auto srcFolder = create_dir(temporary_dir.get_path(), "src");
        create_file(srcFolder / path("test.cpp"), verbose);
        create_file(srcFolder / path("read.cpp"), verbose);
        create_file(srcFolder / path("write.cpp"), verbose);
        create_file(srcFolder / path("write.p"), verbose);
        create_file(srcFolder / path("write.txt"), verbose);
        create_file(srcFolder / path("foo.m"), verbose);
        create_file(srcFolder / path("foo.mat"), verbose);
        create_file(srcFolder / path("foo.p"), verbose);
        create_symlink(srcFolder / path("test.cpp"), srcFolder / path("foo.link.cpp"), verbose);

        auto binFolder = create_dir(temporary_dir.get_path(), "bin");
        create_file(binFolder / path("test"), verbose);

        auto docFolder = create_dir(temporary_dir.get_path(), "doc");
    }
} // namespace ioutils
