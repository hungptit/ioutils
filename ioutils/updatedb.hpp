#pragma once

#include "ioutils/fdwriter.hpp"
#include "ioutils/filesystem.hpp"
#include <limits>
#include <string>
#include <vector>

namespace ioutils {

    struct UpdateDBStreamPolicy {
        template <typename Params> UpdateDBStreamPolicy(Params &&params) : writer(params.database.data()) {}

        static auto is_valid_dir(const char *dname) -> bool { return filesystem::is_valid_dir(dname); }
        void process_file(const Path &parent, const char *stem = nullptr) {
            writer.write(parent.path.data(), parent.path.size());
            if (stem != nullptr) {
                writer.sep();
                writer.write(stem, strlen(stem));
            }
            writer.eol();
        }

        void process_symlink(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

        /**
         * Just store the FIFO paths for now.
         */
        void process_fifo(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

        /**
         * Store the path of character special for now. Not sure if users ever need to
         * know about this type of paths.
         */
        void process_chr(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

        /**
         * Do not have a specific example for this type. Let store its path for now.
         */
        void process_blk(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

        /**
         * Treat socket as file for now. We are generally not interrested in sockets.
         */
        void process_socket(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

        /**
         * A whiteout path is specific to MacOS.
         */
        void process_whiteout(const Path &parent, const char *stem = nullptr) {
            process_file(parent, stem);
        }

        /**
         * TODO: We treat unknown as a file for now. This strategy seem to work well
         * in MacOS.
         */
        void process_unknown(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }

        void process_dir(const std::string &path) {
            writer.write(path.data(), path.size());
            writer.eol();
        }

      private:
        StreamWriter writer;
    };

    struct UpdateDBInputArguments {
        int flags = 0;
        int maxdepth = std::numeric_limits<int>::max();
        bool stats = false;
        bool verbose = false;
        std::string database;           // The output fast-locate database path
        std::vector<std::string> paths; // Saearch paths

        static auto dfs() -> bool { return true; } // Use dfs traversal to explore folders.
        [[nodiscard]] static auto donot_ignore_git() -> bool { return true; }
        [[nodiscard]] static auto follow_symlink() -> bool { return false; }
        [[nodiscard]] static auto ignore_error() -> bool { return false; }

        void print() const;
    };

    void updatedb(UpdateDBInputArguments &params);

} // namespace ioutils
