#include "argparse/argparse.hpp"
#include "fmt/base.h"
#include "hs_compile.h"
#include "ioutils/locate.hpp"
#include "ioutils/search_params.hpp"
#include "version.hpp"
#include <filesystem>
#include <glaze/core/context.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <set>
#include <string>
#include <vector>

namespace {
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

    auto execute_command(int argc, char *argv[]) {
        using namespace ioutils::locate;

        constexpr char LOCATE_COMMAND[] = "locate";
        constexpr char UPDATE_COMMAND[] = "update";
        constexpr char FIND_COMMAND[] = "find";
        constexpr char GREP_COMMAND[] = "grep";

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

        argparse::ArgumentParser program("fast");

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
        argparse::ArgumentParser locate_command(LOCATE_COMMAND);
        locate_command.add_description("Locate folders and files using the locate database.");
        locate_command.add_argument("--database")
            .help(
                "The locate database path. The locate command will use the path provided by the LOCATE_DB "
                "environment variable or the $HOME/.database path if unspecified.")
            .store_into(args.database)
            .default_value("");

        locate_command.add_argument("--regex")
            .help("The regex pattern")
            .store_into(args.regex)
            .default_value("");

        // Update the locate database

        // Find files and folders

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

        // Display the input arguments if specified.
        if (args.verbose) {
            std::string str;
            const auto errcode = glz::write_json(args, str);
            if (errcode == glz::error_code::none) {
                fmt::print("Input arguments: {}\n", str);
            }
        }

        // Update flags and regex_mode
        args.regex_mode =
            (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH) | (args.ignore_case ? HS_FLAG_CASELESS : 0);

        // Process the specified command
        if (program.is_subcommand_used(LOCATE_COMMAND)) {
            if (args.database.empty()) {
                args.database = get_default_locate_database();
            }
            ioutils::locate_files(args);
        } else if (program.is_subcommand_used(UPDATE_COMMAND)) {
            fmt::print("Update the locate database: {}", args.database);
        } else if (program.is_subcommand_used(FIND_COMMAND)) {
            fmt::print("Implement this command: {}", FIND_COMMAND);
        } else {
            fmt::print("Invalid subcommand.\n{}\n", program.help().str());
        }
    }
} // namespace

auto main(int argc, char *argv[]) -> int {
    execute_command(argc, argv);
    return EXIT_SUCCESS;
}
