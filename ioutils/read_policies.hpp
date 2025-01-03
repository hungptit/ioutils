#pragma once

#include <string>
#include <utility>
namespace ioutils {
    template <typename Container> class AppendPolicy {
      public:
        void process(const char *buffer, const size_t len) { data.emplace_back(std::string(buffer, len)); }
        void finalize() {}
        void set_filename(const char * /*unused*/) {}
        auto get_data() -> Container { return std::move(data); }

      private:
        Container data;
    };

    template <> class AppendPolicy<std::string> {
      public:
        void process(const char *buffer, const size_t len, const size_t /*unused*/) {
            data.append(buffer, len);
        }
        auto get_data() -> const std::string & { return data; }
        void finalize() {}
        void set_filename(const char * /*unused*/) {}
        void clear() { data.clear(); }

      private:
        std::string data;
    };

} // namespace ioutils
