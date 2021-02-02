//
// Created by Hedzr Yeh on 2021/2/2.
//

#ifndef CMDR_CXX11_CMDR_DBG_HH
#define CMDR_CXX11_CMDR_DBG_HH

#include <execinfo.h>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

namespace cmdr::debug {

#if __cplusplus < 201402
    template<class T>
    std::string
    type_name() {
        typedef typename std::remove_reference<T>::type TR;
        std::unique_ptr<char, void (*)(void *)> own(
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                    nullptr, nullptr),
#else
                nullptr,
#endif
                std::free);
        std::string r = own != nullptr ? own.get() : typeid(TR).name();
        if (std::is_const<TR>::value)
            r += " const";
        if (std::is_volatile<TR>::value)
            r += " volatile";
        if (std::is_lvalue_reference<T>::value)
            r += "&";
        else if (std::is_rvalue_reference<T>::value)
            r += "&&";
        return r;
    }
#else
    template<typename T>
    constexpr auto type_name() noexcept {
        std::string_view name = "Error: unsupported compiler", prefix, suffix;
#ifdef __clang__
        name = __PRETTY_FUNCTION__;
        prefix = "auto type_name() [T = ";
        suffix = "]";
#elif defined(__GNUC__)
        name = __PRETTY_FUNCTION__;
        prefix = "constexpr auto type_name() [with T = ";
        suffix = "]";
#elif defined(_MSC_VER)
        name = __FUNCSIG__;
        prefix = "auto __cdecl type_name<";
        suffix = ">(void) noexcept";
#endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }
#endif

#ifdef __clang__
#define __FUNCTION_NAME__ __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
#define __FUNCTION_NAME__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define __FUNCTION_NAME__ __FUNCSIG__
#endif

    // to detect the type of a lambda function, following:
    //   https://stackoverflow.com/a/7943736/6375060

    template<class>
    struct mem_type;

    template<class C, class T>
    struct mem_type<T C::*> {
        typedef T type;
    };

    template<class T>
    struct lambda_func_type {
        typedef typename mem_type<decltype(&T::operator())>::type type;
    };

#if 0
    void main_lambda_compare() {
        auto l = [](int i) { return long(i); };
        typedef lambda_func_type<decltype(l)>::type T;
        static_assert(std::is_same<T, long(int) const>::value, "ok");
    }
#endif


    //

    //

    //


    std::string demangle(const char *name);

    template<class T>
    inline std::string type(const T &t) {
        return demangle(typeid(t).name());
    }

#ifdef __GNUG__
    inline std::string demangle(const char *name) {
        int status = -4; // some arbitrary value to eliminate the compiler warning
        // enable c++11 by passing the flag -std=c++11 to g++
        std::unique_ptr<char, void (*)(void *)> res{
                abi::__cxa_demangle(name, nullptr, nullptr, &status),
                std::free};
        return (status == 0) ? res.get() : name;
    }
#else
    // does nothing if not g++
    inline std::string demangle(const char *name) {
        return name;
    }
#endif

#if defined(__GNUC__) || defined(__clang__)
    template<int max_frames = 63>
    static inline std::vector<std::string> save_stacktrace(int skip = 1) {
        void *addrlist[max_frames + 1];
        int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void *));
        std::vector<std::string> ret;
        if (addrlen == 0) {
            fprintf(stderr, "  <empty, possibly corrupt>\n");
            return ret;
        }

        char **symbollist = backtrace_symbols(addrlist, addrlen);

        for (int i = 1 + skip; i < addrlen; i++) {
            ret.emplace_back(symbollist[i]);
        }

        free(symbollist);
        return ret;
    }

    //
    // # compile with symbols for backtrace
    //     CXXFLAGS=-g
    // # add symbols to dynamic symbol table for backtrace
    //     LDFLAGS=-rdynamic
    //
    // https://panthema.net/2008/0901-stacktrace-demangled/
    /** Print a demangled stack backtrace of the caller function to FILE* out. */
    template<int max_frames = 63>
    static inline void print_stacktrace(FILE *out = stderr, int skip = 1) {
        fprintf(out, "stack trace:\n");

        // storage array for stack trace address data
        void *addrlist[max_frames + 1];

        // retrieve current stack addresses
        int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void *));

        if (addrlen == 0) {
            fprintf(out, "  <empty, possibly corrupt>\n");
            return;
        }

        // resolve addresses into strings containing "filename(function+address)",
        // this array must be free()-ed
        char **symbollist = backtrace_symbols(addrlist, addrlen);

        // allocate string which will be filled with the demangled function name
        size_t funcnamesize = 256;
        char *funcname = (char *) malloc(funcnamesize);

        // iterate over the returned symbol lines. skip the first, it is the
        // address of this function.
        for (int i = 1 + skip; i < addrlen; i++) {
            char *begin_name = nullptr, *begin_offset = nullptr, *end_offset = nullptr;

            // find parentheses and +address offset surrounding the mangled name:
            // ./module(function+0x15c) [0x8048a6d]
            for (char *p = symbollist[i]; *p; ++p) {
                if (*p == '(')
                    begin_name = p;
                else if (*p == '+')
                    begin_offset = p;
                else if (*p == ')' && begin_offset) {
                    end_offset = p;
                    break;
                }
            }

            if (end_offset == nullptr) {
                int sp = 0;
                begin_name = nullptr;
                begin_offset = nullptr;
                // clang: "0   test-app2-c2        0x000000010541a2cc _ZN4cmdr5debugL16print_stacktraceILi63EEEvP7__sFILE + 76"
                for (char *p = symbollist[i]; *p; ++p) {
                    if (p[1] == 0 && begin_offset)
                        end_offset = p + 1;
                    else if (*p == ' ') {
                        sp++;
                        while (*(++p) == ' ')
                            ;
                        if (*p == '+' && begin_name)
                            begin_offset = p - 1;
                        else if (sp == 3) {
                            begin_name = p - 1;
                        }
                    }
                }
            }

            if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
                *begin_name++ = '\0';
                *begin_offset++ = '\0';
                *end_offset = '\0';

                if (*begin_offset == '+') begin_offset++;
                if (*begin_offset == ' ') begin_offset++;

                // mangled name is now in [begin_name, begin_offset) and caller
                // offset in [begin_offset, end_offset). now apply
                // __cxa_demangle():

                int status;
                char *ret = abi::__cxa_demangle(begin_name,
                                                funcname, &funcnamesize, &status);
                if (status == 0) {
                    funcname = ret; // use possibly realloc()-ed string
                    fprintf(out, "  %s : %s+%s\n",
                            symbollist[i], funcname, begin_offset);
                } else {
                    // demangling failed. Output function name as a C function with
                    // no arguments.
                    fprintf(out, "  %s : %s()+%s\n",
                            symbollist[i], begin_name, begin_offset);
                }
            } else {
                // couldn't parse the line? print the whole line.
                fprintf(out, "  %s\n", symbollist[i]);
            }
        }

        free(funcname);
        free(symbollist);
    }
    static inline void print_stacktrace(std::vector<std::string> const &st, FILE *out = stderr) {
        fprintf(out, "stack trace:\n");

        size_t funcnamesize = 256;
        char *funcname = (char *) malloc(funcnamesize);
        for (auto const &str : st) {
            char *begin_name = nullptr, *begin_offset = nullptr, *end_offset = nullptr;
            char *data = const_cast<char *>(str.c_str());

            // find parentheses and +address offset surrounding the mangled name:
            // ./module(function+0x15c) [0x8048a6d]
            for (char *p = data; *p; ++p) {
                if (*p == '(')
                    begin_name = p;
                else if (*p == '+')
                    begin_offset = p;
                else if (*p == ')' && begin_offset) {
                    end_offset = p;
                    break;
                }
            }

            if (end_offset == nullptr) {
                int sp = 0;
                begin_name = nullptr;
                begin_offset = nullptr;
                // clang: "0   test-app2-c2        0x000000010541a2cc _ZN4cmdr5debugL16print_stacktraceILi63EEEvP7__sFILE + 76"
                for (char *p = data; *p; ++p) {
                    if (p[1] == 0 && begin_offset)
                        end_offset = p + 1;
                    else if (*p == ' ') {
                        sp++;
                        while (*(++p) == ' ')
                            ;
                        if (*p == '+' && begin_name)
                            begin_offset = p - 1;
                        else if (sp == 3) {
                            begin_name = p - 1;
                        }
                    }
                }
            }

            if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
                *begin_name++ = '\0';
                *begin_offset++ = '\0';
                *end_offset = '\0';

                if (*begin_offset == '+') begin_offset++;
                if (*begin_offset == ' ') begin_offset++;

                // mangled name is now in [begin_name, begin_offset) and caller
                // offset in [begin_offset, end_offset). now apply
                // __cxa_demangle():

                int status;
                char *ret = abi::__cxa_demangle(begin_name,
                                                funcname, &funcnamesize, &status);
                if (status == 0) {
                    funcname = ret; // use possibly realloc()-ed string
                    fprintf(out, "  %s : %s+%s\n",
                            data, funcname, begin_offset);
                } else {
                    // demangling failed. Output function name as a C function with
                    // no arguments.
                    fprintf(out, "  %s : %s()+%s\n",
                            data, begin_name, begin_offset);
                }
            } else {
                // couldn't parse the line? print the whole line.
                fprintf(out, "  %s\n", data);
            }
        }
        free(funcname);
    }
#endif

} // namespace cmdr::debug

namespace cmdr::exception {
    class cmdr_exception : public std::runtime_error {
        std::string msg;
        std::vector<std::string> st;

    public:
        cmdr_exception(const std::string &arg, const char *file, int line)
            : std::runtime_error(arg) {
            std::ostringstream o;
            o << arg << "  " << file << ":" << line;
            msg = o.str();
            auto v = cmdr::debug::save_stacktrace(2);
            st.swap(v);
        }
        ~cmdr_exception() noexcept override = default;
        [[nodiscard]] const char *what() const noexcept override {
            return msg.c_str();
        }
        std::vector<std::string> const &stacktrace() const { return st; }
    };
#define cmdr_throw_line(arg) throw cmdr::exception::cmdr_exception(arg, __FILE__, __LINE__);
} // namespace cmdr::exception

namespace cmdr::debug {
    // unwrap nested exceptions, printing each nested exception to
    // std::cerr
    inline void dump_stacktrace(const char *file, int line, const std::exception &e, std::size_t depth = 0) {
        std::cerr << "  [EX][ERR]: " // << demangle(typeid(e).name()) << ", "
                  << std::string(depth, ' ') << e.what() << '\n';
        if (depth == 0 && "cmdr::exception::cmdr_exception" == type(e)) {
            print_stacktrace(((cmdr::exception::cmdr_exception const *) (&e))->stacktrace());
        }
        try {
            std::rethrow_if_nested(e);
        } catch (const std::exception &nested) {
            dump_stacktrace(file, line, nested, depth + 1);
        }
#if defined(__GNUC__) || defined(__clang__)
        if (depth == 0) {
            std::cerr << "- [CAPTURED]: " << file << ":" << line << '\n';
            print_stacktrace();
        }
#endif
    }
#define CMDR_DUMP_STACK_TRACE(e) cmdr::debug::dump_stacktrace(__FILE__, __LINE__, e)

} // namespace cmdr::debug
#endif //CMDR_CXX11_CMDR_DBG_HH
