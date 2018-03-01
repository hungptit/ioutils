#pragma once

#include <string>
#include <ctime>

namespace utils {
    // A simple parser for timestamp.
    class TimeParser {
	public:
        TimeParser(const std::string &fmt) : format(fmt) {}
        std::time_t operator()(const char *timestr) {
            strptime(timestr, format.c_str(), &timestamp);
            timestamp.tm_isdst = 0; // TODO: Disable day light time saving for now.
            return mktime(&timestamp);
        }

	private:
        std::string format;
        struct tm timestamp;
    };

	struct TimePrinter {
        TimePrinter(const std::string &fmt) : format(fmt) {}

        void operator()(const std::time_t t) {
            struct tm *timestamp = localtime(&t);
            strftime(buffer, buffer_size, format.c_str(), timestamp);
        }

        void operator()(const struct tm *timestamp) {
            strftime(buffer, buffer_size, format.c_str(), timestamp);
        }

        std::string format;
        static constexpr size_t buffer_size = 255;
        char buffer[buffer_size];
    };
}
