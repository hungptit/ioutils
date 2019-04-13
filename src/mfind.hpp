#pragma once

#include "fdwriter.hpp"
#include "filesystem.hpp"

namespace ioutils {
    namespace mfind {
        static const std::string FIFO_COLOR = "\033[1;32m";
        static const std::string DIR_COLOR = "\033[1;91m"; // Bold, light red
        static const std::string CHR_COLOR = "\033[2;33m";
        static const std::string SYMLINK_COLOR = "\033[2;34m"; // Blink, Blue
        static const std::string BLK_COLOR = "\033[1;35m";
        static const std::string SOCK_COLOR = "\033[1;36m";
        static const std::string WHT_COLOR = "\033[4;37m";
        static const std::string FILE_COLOR = "\033[2;97m"; // Normal, white
        static const std::string RESET_COLOR = "\033[0m";

        struct SimplePolicy {
          public:
            template <typename Params>
            SimplePolicy(Params &&params)
                : color(params.color()), ignore_dir(params.ignore_dir()), ignore_file(params.ignore_file()),
                  ignore_symlink(params.ignore_symlink()), writer(StreamWriter::STDOUT) {}

            ~SimplePolicy() {
                if (color) {
                    writer.write(RESET_COLOR.data(), RESET_COLOR.size());
                }

                // fmt::print(stderr, "Number of folders: {}\n", number_of_dirs);
                // fmt::print(stderr, "Number of files: {}\n", number_of_files);
                // fmt::print(stderr, "Number of symlinks: {}\n", number_of_symlinks);
            }

          protected:
            bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
            void process_file(const Path &parent, const char *stem) {
                if (ignore_file) return;
                if (color) {
                    writer.write(FILE_COLOR.data(), FILE_COLOR.size());
                }
                writer.write(parent.path.data(), parent.path.size());
                writer.sep();
                writer.write(stem, strlen(stem));
                writer.eol();
                ++number_of_files;
            }

            void process_file(const Path &parent) {
                if (ignore_file) return;
                if (color) {
                    writer.write(FILE_COLOR.data(), FILE_COLOR.size());
                }
                writer.write(parent.path.data(), parent.path.size());
                writer.eol();
                ++number_of_files;
            }

            void process_symlink(const Path &parent, const char *stem = nullptr) {
                if (ignore_symlink) return;
                if (color) {
                    writer.write(SYMLINK_COLOR.data(), SYMLINK_COLOR.size());
                }
                writer.write(parent.path.data(), parent.path.size());
                if (stem != nullptr) {
                    writer.sep();
                    writer.write(stem, strlen(stem));
                }
                reset_color();
                writer.eol();
                ++number_of_symlinks;
            }

            // Note: The interface of this method is different because we cannot reuse caller's Path object.
            void process_dir(const std::string &parent) {
                if (ignore_dir) return;
                if (color) {
                    writer.write(DIR_COLOR.data(), DIR_COLOR.size());
                }
                writer.write(parent.data(), parent.size());
                reset_color();
                writer.eol();
                ++number_of_dirs;
            }

            void process_fifo(const Path &parent, const char *stem = nullptr) {
                if (ignore_fifo) return;
                if (color) {
                    writer.write(FIFO_COLOR.data(), FIFO_COLOR.size());
                }
                process_path(parent, stem);
                reset_color();
            }

            void process_chr(const Path &parent, const char *stem = nullptr) {
                if (ignore_chr) return;
                if (color) {
                    writer.write(CHR_COLOR.data(), CHR_COLOR.size());
                }
                process_path(parent, stem);
                reset_color();
            }

            void process_block(const Path &parent, const char *stem = nullptr) {
                if (ignore_block) return;
                if (color) {
                    writer.write(BLK_COLOR.data(), BLK_COLOR.size());
                }
                process_path(parent, stem);
                reset_color();
            }

            void process_socket(const Path &parent, const char *stem = nullptr) {
                if (ignore_socket) return;
                if (color) {
                    writer.write(SOCK_COLOR.data(), SOCK_COLOR.size());
                }
                process_path(parent, stem);
            }

            void process_whiteout(const Path &parent, const char *stem = nullptr) {
                if (ignore_whiteout) return;
                if (color) {
                    writer.write(WHT_COLOR.data(), WHT_COLOR.size());
                }
                process_path(parent, stem);
            }

          private:
            bool color;
            bool ignore_dir;
            bool ignore_file;
            bool ignore_symlink;

            bool ignore_fifo = false;
            bool ignore_chr = false;
            bool ignore_block = false;
            bool ignore_socket = false;
            bool ignore_whiteout = false;

            StreamWriter writer;
            int number_of_files = 0;
            int number_of_dirs = 0;
            int number_of_symlinks = 0;

            void reset_color() {
                if (color) {
                    writer.write(RESET_COLOR.data(), RESET_COLOR.size());
                }
            }

            // Print out a given path.
            void process_path(const Path &parent, const char *stem) {
                writer.write(parent.path.data(), parent.path.size());
                if (stem != nullptr) {
                    writer.sep();
                    writer.write(stem, strlen(stem));
                }
                writer.eol();
            }
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
                    writer.eol();
                }
            }

            void process_file(const Path &parent) {
                if (ignore_file) return;
                if (matcher.is_matched(parent.path.data(), parent.path.size())) {
                    if (color) {
                        writer.write(FILE_COLOR.data(), FILE_COLOR.size());
                    }
                    writer.write(parent.path.data(), parent.path.size());
                    writer.eol();
                }
            }

            void process_symlink(const Path &parent, const char *stem) {
                if (ignore_symlink) return;

                // Construct the current path.
                buffer = parent.path;
                if (stem != nullptr) {
                    buffer.push_back('/');
                    buffer.append(stem);
                }

                if (matcher.is_matched(buffer.data(), buffer.size())) {
                    if (color) {
                        writer.write(SYMLINK_COLOR.data(), SYMLINK_COLOR.size());
                    }
                    writer.write(buffer.data(), buffer.size());
                    writer.eol();
                }
            }

            // TODO: Figure out how to fix the duplicated code without doing memory copy.
            void process_symlink(const Path &parent) {
                if (ignore_symlink) return;
                if (matcher.is_matched(parent.path.data(), parent.path.size())) {
                    if (color) {
                        writer.write(SYMLINK_COLOR.data(), SYMLINK_COLOR.size());
                    }
                    writer.write(parent.path.data(), parent.path.size());
                    writer.eol();
                }
            }

            void process_chr(const Path &parent, const char *stem = nullptr) {
                if (ignore_chr) return;
                if (stem != nullptr) {
                    buffer = parent.path;
                    buffer.push_back(SEP);
                    buffer.append(stem);
                    if (!matcher.is_matched(buffer.data(), buffer.size())) return;
                } else {
                    if (!matcher.is_matched(parent.path.data(), parent.path.size())) return;
                }
                if (color) {
                    writer.write(SYMLINK_COLOR.data(), SYMLINK_COLOR.size());
                }
                writer.write(parent.path.data(), parent.path.size());
                writer.eol();
            }

            void process_dir(const std::string &p) {
                if (!ignore_dir) return;
                if (matcher.is_matched(p.data(), p.size())) {
                    if (color) {
                        writer.write(DIR_COLOR.data(), DIR_COLOR.size());
                    }
                    writer.write(buffer.data(), buffer.size());
                    writer.eol();
                }
            }

          private:
            std::string buffer;
            Matcher matcher;

            // Display functionality related flags.
            bool ignore_file;
            bool ignore_dir;
            bool ignore_symlink;
            bool ignore_fifo = false;
            bool ignore_block = false;
            bool ignore_chr = false;
            bool ignore_socket = false;
            bool ignore_whiteout = false;
            bool color;

            StreamWriter writer;
            const char EOL = '\n';
            const char SEP = '/';
        };
    } // namespace mfind
} // namespace ioutils
