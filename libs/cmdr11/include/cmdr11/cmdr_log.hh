//
// Created by Hedzr Yeh on 2021/1/13.
//

#ifndef CMDR_CXX11_CMDR_LOG_HH
#define CMDR_CXX11_CMDR_LOG_HH

namespace cmdr::log {
    class Logger {
    public:
        void print(const char *fmt, void *args);
    };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

    inline void Logger::print(const char *fmt, void *args) {
    }

#pragma clang diagnostic pop

    // Logger log;
} // namespace cmdr::log

#endif //CMDR_CXX11_CMDR_LOG_HH
