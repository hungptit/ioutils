#pragma once

#include "fdwriter.hpp"
#include "filesystem.hpp"

namespace ioutils {
    namespace mfind {
        static const std::string DIR_COLOR = "\033[1;32m";
        static const std::string FILE_COLOR = "\033[1;39m";
        static const std::string SYMLINK_COLOR = "\033[1;34m";
        static const std::string RESET_COLOR = "\033[0m";

        struct SimplePolicy {
          public:
            template <typename Params>
            SimplePolicy(Params &&params)
                : color(params.color()), ignore_dir(params.ignore_dir()),
                  ignore_file(params.ignore_file()), ignore_symlink(params.ignore_symlink()),
                  writer(StreamWriter::STDOUT) {}

            ~SimplePolicy() {
                if (color) {
                    writer.write(RESET_COLOR.data(), RESET_COLOR.size());
                }
            }

          protected:
            bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
            void process_file(const Path &parent, const char *stem) {
                if (ignore_file) return;
                if (color) {
                    writer.write(FILE_COLOR.data(), FILE_COLOR.size());
                }
                writer.write(parent.path.data(), parent.path.size());
                writer.put(SEP);
                writer.write(stem, strlen(stem));
                writer.put(EOL);
                ++number_of_files;
            }

            void process_file(const Path &parent) {
                if (ignore_file) return;
                if (color) {
                    writer.write(FILE_COLOR.data(), FILE_COLOR.size());
                }
                writer.write(parent.path.data(), parent.path.size());
                writer.put(EOL);
                ++number_of_files;
            }

            void process_symlink(const Path &parent, const char *stem) {
                if (ignore_symlink) return;
                if (color) {
                    writer.write(SYMLINK_COLOR.data(), SYMLINK_COLOR.size());
                }
                writer.write(parent.path.data(), parent.path.size());
                writer.put(SEP);
                writer.write(stem, strlen(stem));
                writer.put(EOL);
                ++number_of_symlinks;
            }
            void process_dir(const std::string &parent) {
                if (ignore_dir) return;
                if (color) {
                    writer.write(DIR_COLOR.data(), DIR_COLOR.size());
                }
                writer.write(parent.data(), parent.size());
                writer.put(EOL);
                ++number_of_dirs;
            }

          private:
            bool color;
            bool ignore_dir;
            bool ignore_file;
            bool ignore_symlink;
            StreamWriter writer;
            int number_of_files = 0;
            int number_of_dirs = 0;
            int number_of_symlinks = 0;
            const char EOL = '\n';
            const char SEP = '/';
        };

        template <typename Matcher> class RegexPolicy {
          public:
            template <typename Params>
            RegexPolicy(Params &&params)
                : buffer(), matcher(params.regex, params.regex_mode), ignore_file(params.ignore_file()),
                  ignore_dir(params.ignore_dir()), ignore_symlink(params.ignore_symlink()), color(params.color()),
                  writer(StreamWriter::STDOUT) {
                buffer.reserve(1023);
            }

          protected:
            bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }

            void process_file(const Path &parent, const char *stem) {
                if (ignore_file) return;
                buffer = parent.path + "/" + stem;
                if (matcher.is_matched(buffer.data(), buffer.size())) {
                    if (color) {
                        writer.write(FILE_COLOR.data(), FILE_COLOR.size());
                    }
                    writer.write(buffer.data(), buffer.size());
                    writer.put(EOL);
                }
            }

            void process_file(const Path &parent) {
                if (ignore_file) return;
                if (matcher.is_matched(parent.path.data(), parent.path.size())) {
                    if (color) {
                        writer.write(FILE_COLOR.data(), FILE_COLOR.size());
                    }
                    writer.write(parent.path.data(), parent.path.size());
                    writer.put(EOL);
                }
            }

            void process_symlink(const Path &parent, const char *stem) {
                if (ignore_symlink) return;
                buffer = parent.path + "/" + stem;
                if (matcher.is_matched(buffer.data(), buffer.size())) {
                    if (color) {
                        writer.write(SYMLINK_COLOR.data(), SYMLINK_COLOR.size());
                    }
                    writer.write(buffer.data(), buffer.size());
                    writer.put(EOL);
                }
            }

            void process_dir(const std::string &p) {
                if (!ignore_dir) return;
                if (matcher.is_matched(p.data(), p.size())) {
                    if (color) {
                        writer.write(DIR_COLOR.data(), DIR_COLOR.size());
                    }
                    writer.write(buffer.data(), buffer.size());
                    writer.put(EOL);
                }
            }

          private:
            std::string buffer;
            Matcher matcher;

            // Display functionality related flags.
            bool ignore_file;
            bool ignore_dir;
            bool ignore_symlink;
            bool color;

            StreamWriter writer;
            const char EOL = '\n';
            const char SEP = '/';
        };
    } // namespace mfind
} // namespace ioutils
