#include "clara.hpp"

#include "ioutils/find.hpp"
#include "ioutils/search.hpp"
#include "ioutils/search_params.hpp"
#include "matchers.hpp"
#include "regex_matchers.hpp"

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
