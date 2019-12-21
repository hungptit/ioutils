#pragma once

#include "temporary_dir.hpp"
#include <iostream>
#include <fstream>

namespace {
    namespace fs = std::filesystem;
    class TestData {
      public:
        using path = fs::path;
        explicit TestData(const bool verbose = false) : temporary_dir() { init(verbose); }

        auto get_path() {
            return temporary_dir.get_path();
        }

      private:
        ioutils::TemporaryDirectory temporary_dir;

        void create_file(const path &aFile, const bool verbose = false) {
            std::ofstream outfile(aFile.string());
            outfile << "Fake data for testing.\n";
            outfile.close();
            if (fs::exists(aFile)) {
                if (verbose) std::cout << aFile.string() << " is created" << std::endl;
            } else {
                std::cerr << "Cannot create " << aFile.string() << std::endl;
            }
        }

        void create_symlink(const path &p, const path &symlink, const bool verbose = false) {
            fs::create_symlink(p, symlink);
            if (verbose) {
                if (verbose) std::cout << symlink.string() << " is linked" << std::endl;
            }
        }

        path create_dir(const path &rootFolder, const path &aPath) {
            auto fullPath = rootFolder / aPath;
            fs::create_directories(fullPath);
            return fullPath;
        }

        void init(const bool verbose) {
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
    };
} // namespace
