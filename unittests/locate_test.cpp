#include "ioutils/locate.hpp"
#include "ioutils/updatedb.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

namespace {
    const std::string database = "test_database";
    const std::vector<std::string> paths = {"../include", "../unittests///"};
} // namespace

TEST_CASE("updatedb input parameters") {
    ioutils::UpdateDBInputArguments params;
    params.database = database;
    params.paths = paths;
    params.print();
    SUBCASE("Create the test database") { ioutils::updatedb(params); }
}

TEST_CASE("Locate files") {
    SUBCASE("List all files") {
        ioutils::LocateInputArguments params;
        params.databases = {database};
        params.pattern = "";
        params.print();
        ioutils::locate_files(params);
    }

    SUBCASE("Locate a file") {
        ioutils::LocateInputArguments params;
        params.databases = {database};
        params.pattern = ".*cpp$";
        params.regex_mode = HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH;
        params.prefix = "====>";
        params.print();
        ioutils::locate_files(params);
    }

    SUBCASE("Locate a file using an invert match") {
        ioutils::LocateInputArguments params;
        params.databases = {database};
        params.pattern = ".*hpp$";
        params.regex_mode = HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH;
        params.print();
        params.flags |= ioutils::locate::INVERT_MATCH;
        ioutils::locate_files(params);
    }
}
