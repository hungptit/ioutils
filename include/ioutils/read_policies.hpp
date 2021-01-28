#pragma once
#include <deque>
#include <string>
namespace ioutils {
    template <typename Container> class AppendPolicy {
      public:
        void process(const char *buffer, const size_t len) { data.emplace_back(std::string(buffer, len)); }
        void finalize() {}
        void set_filename(const char *) {}
        Container get_data() { return std::move(data); }

      private:
        Container data;
    };

    template <> class AppendPolicy<std::string> {
      public:
        void process(const char *buffer, const size_t len, const size_t) { data.append(buffer, len); }
        const std::string &get_data() { return data; }
        void finalize() {}
        void set_filename(const char *) {}
        void clear() { data.clear(); }

      private:
        std::string data;
    };

} // namespace ioutils
