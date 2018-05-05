#include <fstream>
#include <iostream>

#include "fmt/format.h"
#include "search.hpp"
#include "temporary_dir.hpp"
#include "test_data.hpp"

#include "fmt/format.h"

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"
using Catch::Matchers::Equals;

using namespace boost;

TEST_CASE("Utility function", "basic") {
    TestData test(true);
    auto tmpdir = test.get_path().
    fmt::print("{}\n", tmpdir.string());
    const std::string p = tmpdir.get_path().string();
    struct stat props;
    stat(p.data(), &props);

    CHECK(filesystem::exists(tmpdir.get_path()));
    CHECK(ioutils::filesystem::exists(p.data()));
    CHECK(ioutils::filesystem::is_directory(props.st_mode));
    CHECK(!ioutils::filesystem::is_regular_file(props.st_mode));
    CHECK(!ioutils::filesystem::is_symlink(props.st_mode));


}
