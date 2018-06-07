#pragma once
#include <string>
namespace ioutils {
    class AppendPolicy {
      public:
        void process(const char *buffer, const size_t len) { _data.append(buffer, len); }
        std::string data() { return std::move(_data); }

      private:
        std::string _data;
    };

} // namespace ioutils
