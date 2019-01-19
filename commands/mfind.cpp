#include "cereal/archives/json.hpp"
#include "clara.hpp"
#include "fmt/format.h"
#include "search_params.hpp"
#include "regex_policies.hpp"
#include "search.hpp"
#include "search_policies.hpp"
#include "simple_search_policy.hpp"
#include "utilities.hpp"
#include "utils/copyright.hpp"
#include "utils/matchers.hpp"
#include "utils/regex_matchers.hpp"
#include <dirent.h>

namespace {
    template <typename Params> void search(Params &&params) {
        // Search for files based on given constraints.
        if (params.path_regex.empty()) {
            using Policy = ioutils::SimplePolicy;
            ioutils::FileSearch<Policy> search(params);
            search.dfs(params.paths);
        } else {
            // Find desired files and folders
            if (!params.inverse_match()) {
                using Matcher = utils::hyperscan::RegexMatcher;
                using Policy = ioutils::RegexPolicy<Matcher>;
                ioutils::FileSearch<Policy> search(params);
                search.dfs(params.paths);
            } else {
                using Matcher = utils::hyperscan::RegexMatcherInv;
                using Policy = ioutils::RegexPolicy<Matcher>;
                ioutils::FileSearch<Policy> search(params);
                search.dfs(params.paths);
            }
        }
    }
} // namespace

int main(int argc, char *argv[]) {
    auto params = ioutils::search::parse_input_arguments(argc, argv);
    search(params);
    return EXIT_SUCCESS;
}
