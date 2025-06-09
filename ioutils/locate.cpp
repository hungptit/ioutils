#include "ioutils/locate.hpp"
#include "boost/iostreams/device/mapped_file.hpp"
#include "fmt/base.h"
#include "fmt/ranges.h"
#include "ioutils/fdwriter.hpp"
#include "regex_matchers.hpp"
#include <filesystem>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>

namespace ioutils {
    void LocateInputArguments::print() const {
        fmt::print("verbose: {}\n", verbose());
        fmt::print("info: {}\n", info());
        fmt::print("invert-match: {}\n", invert_match());
        fmt::print("exact-match: {}\n", exact_match());
        fmt::print("ignore-case: {}\n", ignore_case());
        fmt::print("regex-mode: {}\n", regex_mode);
        fmt::print("Search pattern: '{}'\n", pattern);
        fmt::print("path prefix: '{}'\n", prefix);
        fmt::print("File information databases: [\"{}\"]\n", fmt::join(databases, "\",\""));
    }

    void locate_files(const ioutils::LocateInputArguments &params) {
        if (params.pattern.empty()) {
            using GrepAlg = ioutils::StreamReader<PrintAllPolicy>;
            GrepAlg grep(params);
            for (auto db : params.databases) {
                grep(db.data());
            }
        } else {
            if (!params.invert_match()) {
                using Matcher = utils::hyperscan::RegexMatcher;
                find_matched_files<Matcher>(params);
            } else {
                using Matcher = utils::hyperscan::RegexMatcherInv;
                find_matched_files<Matcher>(params);
            }
        }
    }

    void locate_files_all(const ioutils::Parameters &args) {
        if (!std::filesystem::exists(args.database)) {
            throw std::runtime_error(
                std::format("Cannot open the locate database file: {}", args.database));
        }
        StreamWriter console;
        boost::iostreams::mapped_file mmap(args.database);
        const auto *begin = mmap.const_data();
        const auto *ptr = begin;
        const auto *const end = begin + mmap.size();
        constexpr char EOL = '\n';
        while ((ptr = static_cast<const char *>(memchr(begin, EOL, end - begin))) != nullptr) {
            console.write(begin, ptr - begin + 1);
            begin = ptr + 1;
        }
    }

    void locate_files_regex(const ioutils::Parameters &args) {
        if (!std::filesystem::exists(args.database)) {
            throw std::runtime_error(
                std::format("Cannot open the locate database file: {}", args.database));
        }
        StreamWriter console;
        boost::iostreams::mapped_file mmap(args.database);
        const auto *begin = mmap.const_data();
        const auto *ptr = begin;
        const auto *const end = begin + mmap.size();
        constexpr char EOL = '\n';
        while ((ptr = static_cast<const char *>(memchr(begin, EOL, end - begin))) != nullptr) {
            std::string_view line(begin, ptr - begin + 1);
            if (line.contains(args.regex)) {
                console.write(begin, ptr - begin + 1);
            }
            begin = ptr + 1;
        }
    }

    void locate_files(const ioutils::Parameters &args) {
        if (!std::filesystem::exists(args.database)) {
            throw std::runtime_error(
                std::format("Cannot open the locate database file: {}", args.database));
        }

        args.regex.empty() ? locate_files_all(args) : locate_files_regex(args);
    }
} // namespace ioutils
