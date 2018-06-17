#pragma once
#include <deque>
#include <string>
namespace ioutils {
    template <typename Container>
    class AppendPolicy {
    public:
        void process(const char *buffer, const size_t len) {
            _data.emplace_back(std::string(buffer, len));
        }
        Container data() { return std::move(_data); }

    private:
        Container _data;
    };

    template <> class AppendPolicy<std::string> {
      public:
        void process(const char *buffer, const size_t len) { _data.append(buffer, len); }
        std::string data() { return std::move(_data); }

      private:
        std::string _data;
    };


} // namespace ioutils
