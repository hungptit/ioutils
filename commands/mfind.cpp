#include "clara.hpp"
#include "fmt/format.h"
#include "regex_policies.hpp"
#include "search.hpp"
#include "search_policies.hpp"
#include "simple_policy.hpp"
#include "utilities.hpp"
#include "utils/copyright.hpp"
#include "utils/matchers.hpp"
#include "utils/regex_matchers.hpp"
#include <dirent.h>
#include "cereal/archives/json.hpp"

namespace {
    void copyright() {
        fmt::print("{}\n", "mfind version 0.1.0");
        fmt::print("{}\n", "Hung Dang <hungptit@gmail.com>");
    }

    struct SearchParams {
        bool inverse_match = false;                 // Select non-matching lines
        bool verbose = false;                       // Display verbose information.
        int type = ioutils::DisplayType::DISP_NONE; // Display type
        int regex_mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(inverse_match), CEREAL_NVP(verbose), CEREAL_NVP(type),
               CEREAL_NVP(regex_mode));
        }
    };

    
    
    struct InputParams 
    std::string pattern;            // Input patterns
    std::vector<std::string> paths; // Input files and folders
    int flags;
    int regex_mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
    
    template <typename Archive> void serialize(Archive &ar) {
        ar(CEREAL_NVP(pattern), CEREAL_NVP(paths)
           CEREAL_NVP(parameters));
    }
    ;


    template <typename Policy, typename Params> void search(Params &&params) {
        using Search = typename ioutils::FileSearch<Policy>;
        Search search(params);
        search.dfs(params.paths);
    }
} // namespace

int main(int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);

    // Search for files based on given constraints.
    if (params.pattern.empty()) {
        using Policy = ioutils::SimplePolicy;
        search<Policy>(params);
    } else {
        // // Find desired files and folders
        // if (!params.parameters.inverse_match) {
        //     using Matcher = utils::hyperscan::RegexMatcher;
        //     using Policy = ioutils::RegexPolicy<Matcher, decltype(params.parameters)>;
        //     search<Policy>(params);
        // } else {
        //     using Matcher = utils::hyperscan::RegexMatcherInv;
        //     using Policy = ioutils::RegexPolicy<Matcher, decltype(params.parameters)>;
        //     search<Policy>(params);
        // }
    }

    return EXIT_SUCCESS;
}
