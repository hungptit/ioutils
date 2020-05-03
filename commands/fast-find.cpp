#include "clara.hpp"
#include "find.hpp"
#include "search.hpp"
#include "search_params.hpp"
#include "search_policies.hpp"
#include "utils/matchers.hpp"
#include "utils/regex_matchers.hpp"
#include <dirent.h>

namespace {
    template <typename Params> void search(Params &&params) {
        if (params.path_regex.empty()) {
            using Policy = ioutils::find::SimplePolicy;
            ioutils::filesystem::DefaultSearch<Policy> search(params);
            search.traverse(params.paths);
        } else {
            if (!params.invert_match()) {
                using Matcher = utils::hyperscan::RegexMatcher;
                using Policy = ioutils::find::RegexPolicy<Matcher>;
                ioutils::filesystem::DefaultSearch<Policy> search(params);
                search.traverse(params.paths);
            } else {
                using Matcher = utils::hyperscan::RegexMatcherInv;
                using Policy = ioutils::find::RegexPolicy<Matcher>;
                ioutils::filesystem::DefaultSearch<Policy> search(params);
                search.traverse(params.paths);
            }
        }
    }
} // namespace

int main(int argc, char *argv[]) {
    search(ioutils::search::parse_input_arguments(argc, argv));
    return EXIT_SUCCESS;
}
