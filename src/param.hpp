#pragma once

#include "cereal/archives/json.hpp"
#include "fmt/format.h"
#include <string>

namespace ioutils {
    enum PARAMS : uint32_t {
        VERBOSE = 1,
        INVERSE_MATCH = 1 << 1,
        COLOR = 1 << 2,
        IGNORE_FILE = 1 << 3,
        IGNORE_DIR = 1 << 4,
        IGNORE_SYMLINK = 1 << 5,
        FOLLOW_SYMLINK = 1 << 6,
    };

    struct SearchParams {
        int flag;
        int regex_mode;
        std::string path_regex;
        std::vector<std::string> paths;

        bool verbose() { return (flag & PARAMS::VERBOSE) > 0; }
        bool inverse_match() { return (flag & PARAMS::INVERSE_MATCH) > 0; }
        bool color() { return (flag & PARAMS::COLOR) > 0; }

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(flag), CEREAL_NVP(regex_mode), CEREAL_NVP(path_regex),
               CEREAL_NVP(paths));
        }
    };

    SearchParams parse_input_arguments(int argc, char *argv[]) {
        SearchParams params;
        std::vector<std::string> paths;
        bool help = false;
        bool ignore_case = false;
        bool ignore_dir = false;
        bool ignore_file = false;
        bool ignore_symlink = false;
        bool follow_link = false;
        bool color = false;

        std::string begin_time, end_time;

        auto cli =
            clara::Help(help) |
            clara::Opt(params.parameters.verbose)["-v"]["--verbose"](
                "Display verbose information") |
            clara::Opt(ignore_case)["-i"]["--ignore-case"]("Ignore case") |
            clara::Opt(params.parameters.inverse_match)["-u"]["--inverse-match"](
                "Select lines that do not match specified pattern.") |
            clara::Opt(ignore_file)["--ignore-file"]("Ignore files.") |
            clara::Opt(ignore_dir)["--ignore-dir"]("Ignore folders.") |
            clara::Opt(ignore_symlink)["--ignore-symlink"]("Ignore symlink.") |
            clara::Opt(color)["-c"]["--color"]("Print out color text.") |
            clara::Opt(params.pattern, "pattern")["-e"]["--pattern"]("Search pattern.") |

            // Unsupported options
            clara::Opt(follow_link, "follow-link")["--follow-link"]("Follow symbolic links.") |
            clara::Opt(begin_time, "newer")["--newer"](
                "Display paths that are newer than a given timestamp.") |
            clara::Opt(end_time, "older")["--older"](
                "Display paths that are older than a given timestamp.") |

            // Required arguments.
            clara::Arg(paths, "paths")("Search paths");

        auto result = cli.parse(clara::Args(argc, argv));
        if (!result) {
            fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
            exit(EXIT_FAILURE);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            copyright();
            fmt::print("{}", oss.str());
            exit(EXIT_SUCCESS);
        }

        // Need to more extensive path extension here using path_regex.
        if (paths.empty()) {
            params.paths.push_back(".");
        } else {
            for (auto p : paths) {
                ioutils::remove_trailing_slash(p);
                params.paths.emplace_back(p);
            }
        }

        // Init the mode for our regular expression engine.
        if (ignore_case) {
            params.regex_mode |= HS_FLAG_CASELESS;
        }

        // Parse the display type
        params.flag = ignore_file * PARAMS::IGNORE_FILE | ignore_dir * PARAMS::IGNORE_DIR |
                      ignore_symlink * PARAMS::DISP_SYMLINK + color * PARAMS : b : DISP_COLOR;

        // Display input arguments in JSON format if verbose flag is on
        if (params.parameters.verbose) {
            std::stringstream ss;
            {
                cereal::JSONOutputArchive ar(ss);
                ar(cereal::make_nvp("Input arguments", params));
            }
            fmt::print("{}\n", ss.str());
        }

        return params;
    }

} // namespace ioutils
