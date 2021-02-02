//
// Created by Hedzr Yeh on 2021/1/13.
//

#ifndef CMDR_CXX11_CMDR_LOG_HH
#define CMDR_CXX11_CMDR_LOG_HH

#include "cmdr_terminal.hh"
#include "cmdr_utils.hh"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <tuple>


namespace cmdr::log {

    namespace detail {
        class Log final : public util::singleton<Log> {
        public:
            explicit Log(typename util::singleton<Log>::token) {}
            ~Log() = default;

            cmdr::terminal::colors::colorize _c;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
            template<class... Args>
            void log(const char *fmt, Args const &...args) {
                // std::fprintf(std::stderr)
            }
            void vdebug(const char *file, int line, const char *func,
                        char const *fmt, va_list args) {
                auto t = std::time(nullptr);
                char time_buf[100];
                std::strftime(time_buf, sizeof time_buf, "%D %T", std::gmtime(&t));

                va_list args2;
                va_copy(args2, args);
                std::vector<char> buf(1 + std::vsnprintf(nullptr, 0, fmt, args));
                std::vsnprintf(buf.data(), buf.size(), fmt, args2);
                va_end(args2);

                using namespace cmdr::terminal;
                std::printf("%s"
                            "%s %s:"
                            "%s"
                            " %s  %s:%d "
                            "%s"
                            "(%s)"
                            "%s"
                            "\n",
                            fg_light_gray,
                            time_buf,
                            // _c.dim().s("[debug]").as_string().c_str(),
                            "[debug]",
                            fg_reset_all,
                            buf.data(),
                            file, line,
                            fg_light_gray, func, fg_reset_all);
            }
#pragma clang diagnostic pop
        };
    } // namespace detail

    class log {
    public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

        template<class... Args>
        static void print(const char *fmt, Args const &...args) {
            xlog().template log(fmt, args...);
        }

        // static void vdebug(char const *fmt, va_list args) {
        //     xlog().vdebug(fmt, args);
        // }
        static void debug(char const *fmt, ...) {
            auto t = std::time(nullptr);
            char time_buf[100];
            std::strftime(time_buf, sizeof time_buf, "%D %T", std::gmtime(&t));

            va_list args1;
            va_start(args1, fmt);
            va_list args2;
            va_copy(args2, args1);
            std::vector<char> buf(1 + std::vsnprintf(nullptr, 0, fmt, args1));
            va_end(args1);
            std::vsnprintf(buf.data(), buf.size(), fmt, args2);
            va_end(args2);

            std::printf("%s [debug]: %s\n", time_buf, buf.data());
        }

#pragma clang diagnostic pop

    private:
        static detail::Log &xlog() { return detail::Log::instance(); }
    }; // class log

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

    // inline void Logger::print(const char *fmt, void *args) {
    // }

#pragma clang diagnostic pop

    class holder {
        const char *_file;
        int _line;
        const char *_func;

    public:
        holder(const char *file, int line, const char *func)
            : _file(file)
            , _line(line)
            , _func(func) {}

        void operator()(char const *fmt, ...) {
            va_list va;
            va_start(va, fmt);
            xlog().vdebug(_file, _line, _func, fmt, va);
            va_end(va);
        }

    private:
        static detail::Log &xlog() { return detail::Log::instance(); }
    };
    // Logger log;
} // namespace cmdr::log

#if CMDR_ENABLE_VERBOSE_LOG
// inline void debug(char const *fmt, ...) {
//     va_list va;
//     va_start(va, fmt);
//     cmdr::log::log::vdebug(fmt, va);
//     va_end(va);
// }
#if defined(_MSC_VER)
#define cmdr_verbose_debug(...) cmdr::log::holder(__FILE__, __LINE__, __FUNCSIG__)(__VA_ARGS__)
#else
#define cmdr_verbose_debug(..) cmdr::log::holder(__FILE__, __LINE__, __PRETTY_FUNCTION__)(__VA_ARGS__)
#endif
#else
#define cmdr_verbose_debug(...)                                                               \
    _Pragma("GCC diagnostic push")                                                            \
            _Pragma("GCC diagnostic ignored \"-Wunused-value\"") do { (void) (__VA_ARGS__); } \
    while (0)                                                                                 \
    _Pragma("GCC diagnostic pop")
#endif

#if defined(_DEBUG)
#if defined(_MSC_VER)
#define cmdr_debug(...) cmdr::log::holder(__FILE__, __LINE__, __FUNCSIG__)(__VA_ARGS__)
#else
#define cmdr_debug(...) cmdr::log::holder(__FILE__, __LINE__, __PRETTY_FUNCTION__)(__VA_ARGS__)
#endif
#else
#define debug(...)                                                                            \
    _Pragma("GCC diagnostic push")                                                            \
            _Pragma("GCC diagnostic ignored \"-Wunused-value\"") do { (void) (__VA_ARGS__); } \
    while (0)                                                                                 \
    _Pragma("GCC diagnostic pop")
#endif

#endif //CMDR_CXX11_CMDR_LOG_HH
