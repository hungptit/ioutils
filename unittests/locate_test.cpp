#include "ioutils/locate.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

namespace {
    const std::string database = "test_database";
    const std::vector<std::string> paths = {"../include", "../unittests///"};
} // namespace

TEST_CASE("updatedb input parameters") {
    ioutils::locate::UpdateDBInputArguments params;
    params.database = database;
    params.paths = paths;
    params.print();
    SUBCASE("Create the test database") { ioutils::updatedb(params); }
}

TEST_CASE("Locate files") {
    SUBCASE("List all files") {
        ioutils::locate::LocateInputArguments params;
        params.databases = {database};
        params.pattern = "";
        params.print();
        ioutils::locate_files(params);
    }

    SUBCASE("Locate a file") {
        ioutils::locate::LocateInputArguments params;
        params.databases = {database};
        params.pattern = ".*cpp$";
        params.prefix = "====>";
        params.print();
        ioutils::locate_files(params);
    }

    SUBCASE("Locate a file using an invert match") {
        ioutils::locate::LocateInputArguments params;
        params.databases = {database};
        params.pattern = ".*hpp$";
        params.print();
        params.flags = ioutils::locate::INVERT_MATCH;
        ioutils::locate_files(params);
    }
}