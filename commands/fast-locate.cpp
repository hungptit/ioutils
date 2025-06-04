#include "argparse/argparse.hpp"
#include "fmt/base.h"
#include "hs_compile.h"
#include "ioutils/locate.hpp"
#include "ioutils/search_params.hpp"
#include "ioutils/utilities.hpp"
#include "version.hpp"
#include <filesystem>
#include <glaze/core/context.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <set>
#include <string>
#include <vector>

namespace {
    void display_version() { fmt::print("{} {}\n", "fast-locate ", version); }

    void display_usage() {
        fmt::print("\nExamples:\n");
        fmt::print("\t1. Locate files using the default database:\n");
        fmt::print("\t\tfast-locate 'boos.*qvm.*string.hpp$'\n");
        fmt::print("\t2. Locate files using the specified database:\n");
        fmt::print("\t\tfast-locate -d my_db 'boos.*qvm.*string.hpp$'\n");
    }

    std::string get_default_locate_database() {
        constexpr char LOCATE_DB[] = "LOCATE_DB";
        auto *default_db = std::getenv(LOCATE_DB);

        // Try the environment variable if exist.
        if (default_db) {
            if (std::filesystem::exists(default_db)) {
                return {default_db};
            }
        }

        // Use the database in the HOME folder.
        auto *home_dir = std::getenv("HOME");
        auto db = std::filesystem::path(home_dir) / ".database";
        if (std::filesystem::exists(db)) {
            return db.string();
        }

        // Use the database in the current folder if exist.
        const auto current_dir = std::filesystem::current_path();
        auto local_db = current_dir / ".database";
        if (std::filesystem::exists(local_db)) {
            return local_db.string();
        }

        return "";
    }

    auto parse_input_arguments(int argc, char *argv[]) {
        using namespace ioutils::locate;

        constexpr char LOCATE_COMMAND[] = "locate";
        constexpr char UPDATE_COMMAND[] = "update";
        constexpr char FIND_COMMAND[] = "find";

        ioutils::Parameters args{};

        ioutils::LocateInputArguments params;

        bool version = false;
        bool help = false;
        bool verbose = false;
        bool invert_match = false;
        bool ignore_case = false;
        bool exact_match = false;
        bool regex_match = false;
        bool timer = false;
        std::vector<std::string> dbs;
        std::set<std::string> lookup;

        argparse::ArgumentParser program("ioutils");

        program.add_argument("--verbose")
            .help("Print out extra information")
            .store_into(args.verbose)
            .default_value(false);

        // Find command
        argparse::ArgumentParser find_command("find");
        find_command.add_description("Find folders and files.");

        // Update command
        argparse::ArgumentParser update_command("update");
        update_command.add_description("Update the locate database.");

        // Locate files and folders
        argparse::ArgumentParser locate_command("locate");
        locate_command.add_description("Locate folders and files using the locate database.");
        locate_command.add_argument("--database")
            .help("The locate database")
            .store_into(args.database)
            .default_value("");

        // Add all sub-commands
        program.add_subparser(find_command);
        program.add_subparser(update_command);
        program.add_subparser(locate_command);

        try {
            program.parse_args(argc, argv);
        } catch (const std::exception &err) {
            fmt::print(stderr, "{}\n", err.what());
            std::exit(1);
        }

        // Process the specified command
        if (program.is_subcommand_used(LOCATE_COMMAND)) {
            if (args.database.empty()) {
                args.database = get_default_locate_database();
            }
        }

        // Display the input arguments if specified.
        if (args.verbose) {
            std::string str;
            const auto errcode = glz::write_json(args, str);
            if (errcode == glz::error_code::none) {
                fmt::print("Input arguments: {}\n", str);
            }
        }

        // auto cli =
        //     clara::Help(help) | clara::Opt(verbose)["-v"]["--verbose"]("Display verbose information") |
        //     clara::Opt(timer, "timer")["--timer"]("Display the execution runtime.") |
        //     clara::Opt(version)["--version"]("Display the version of fast-locate.") |
        //     clara::Opt(ignore_case)["-i"]["--ignore-case"]("Ignore case.") |
        //     clara::Opt(invert_match)["-u"]["--invert-match"](
        //         "Display lines that do not match given pattern.") |
        //     clara::Opt(regex_match)["-r"]["--regex"]("Use regular expression matching algorithm") |
        //     clara::Opt(exact_match)["-x"]["--exact-match"]("Use exact match algorithm") |
        //     clara::Opt(params.prefix, "prefix")["--prefix"](
        //         "Path prefix which will be appended to the output paths. You can automatically specify
        //         the " "prefix by setting the FAST_LOCATE_PREFIX environment variable.") |
        //     clara::Opt(dbs, "database")["-d"]["--database"](
        //         "The file information database. This database is generated by fast-updatedb command. You
        //         " "can " "automatically specify the database by setting the FAST_LOCATE_DB environment
        //         variable") |
        //     clara::Arg(params.pattern, "pattern")("Search pattern");

        // auto result = cli.parse(clara::Args(argc, argv));
        // if (!result) {
        //     fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
        //     exit(EXIT_FAILURE);
        // }

        // if (version) {
        //     display_version();
        //     exit(EXIT_SUCCESS);
        // }

        // // Print out the help document.
        // if (help) {
        //     std::ostringstream oss;
        //     oss << cli;
        //     fmt::print("{}", oss.str());
        //     display_usage();
        //     ioutils::copyright();
        //     exit(EXIT_SUCCESS);
        // }

        // if (dbs.empty()) {
        //     auto *default_db = std::getenv("FAST_LOCATE_DB");

        //     // Try the environment variable if exist.
        //     if (default_db) {
        //         if (std::filesystem::exists(default_db)) {
        //             params.databases.emplace_back(default_db);
        //         }
        //     } else {
        //         const auto current_dir = std::filesystem::current_path();
        //         auto local_db = current_dir / ".database";
        //         if (std::filesystem::exists(local_db)) {
        //             params.databases.emplace_back(std::move(local_db));
        //         }
        //     }
        // } else {
        //     for (const auto &item : dbs) {
        //         lookup.emplace(item);
        //     }
        //     for (const auto &item : lookup) {
        //         params.databases.push_back(item);
        //     }
        // }

        // // Try the home folder if the database list is empty
        // if (params.databases.empty()) {
        //     auto *home_dir = std::getenv("HOME");
        //     auto db = std::filesystem::path(home_dir) / ".database";
        //     if (std::filesystem::exists(db)) {
        //         params.databases.emplace_back(std::move(db));
        //     }
        // }

        // // If user does not specify prefix then fallback to the
        // // FAST_LOCATE_PREFIX environment variable.
        // if (params.prefix.empty()) {
        //     auto *const default_prefix = std::getenv("FAST_LOCATE_PREFIX");
        //     if (default_prefix != nullptr) {
        //         params.prefix = std::string(default_prefix);
        //     }
        // }

        // // Update flags and regex_mode
        // exact_match = regex_match ? !regex_match : exact_match;
        // params.flags = static_cast<uint32_t>(verbose) * VERBOSE |
        //                static_cast<uint32_t>(invert_match) * INVERT_MATCH |
        //                static_cast<uint32_t>(exact_match) * EXACT_MATCH |
        //                static_cast<uint32_t>(ignore_case) * IGNORE_CASE;
        // params.regex_mode =
        //     (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH) | (params.ignore_case() ? HS_FLAG_CASELESS : 0);

        // // Display input arguments in JSON format if verbose flag is on
        // if (params.verbose()) {
        //     params.print();
        // }

        return params;
    }

} // namespace

auto main(int argc, char *argv[]) -> int {
    const auto params = parse_input_arguments(argc, argv);
    // ioutils::locate_files(params);
    return EXIT_SUCCESS;
}
