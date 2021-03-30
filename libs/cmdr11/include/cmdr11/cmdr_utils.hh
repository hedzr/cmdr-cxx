//
// Created by Hedzr Yeh on 2021/1/13.
//

#ifndef CMDR_CXX11_CMDR_UTILS_HH
#define CMDR_CXX11_CMDR_UTILS_HH

#include <any>
#include <optional>
#include <variant>

#include <list>
#include <vector>

#include <algorithm>
#include <functional>
#include <utility>

#include <cassert>
#include <stdexcept>

#include <iomanip>
#include <iostream>
#include <sstream>

#include <string>

#include <unordered_map>
#include <unordered_set>

#include <type_traits>
#include <typeindex>
#include <typeinfo>

#include <memory>

#include <chrono>

#ifndef __FS_COMPATIBLE
#define __FS_COMPATIBLE
#ifdef __cpp_lib_filesystem
#include <filesystem>
#else

#if __cplusplus < 201703L
#include <experimental/filesystem>
namespace std {
    namespace filesystem = experimental::filesystem;
}
#else
#include <filesystem>
#endif

#endif
#endif

#include "cmdr_chrono.hh"
#include "cmdr_dbg.hh"
// #include "cmdr_log.hh"


namespace cmdr::util {

    //
#if 0
    template<typename T>
    class Nullable {
    public:
        Nullable() = default;

        virtual ~Nullable() {
            if (_value) delete _value;
        }

    public:
        Nullable(const Nullable &o) { _copy(o); }

        Nullable &operator=(const Nullable &o) {
            _copy(o);
            return *this;
        }

        Nullable &operator=(const T &o) {
            this->_value = o;
            return *this;
        }

    private:
        void _copy(const Nullable &o) {
            this->_value = o._value;
        }

    public:
        T &val() { return *_value; }

        const T &val() const { return *_value; }

        void val(T &&v) {
            if (!_value)
                _value = new T;
            (*_value) = v;
        }

        explicit operator T() const { return val(); }

        explicit operator T() { return val(); }

        // operator ->
        // operator *

        // operator ==, !=, >=, <=, >, <, <=>
        // operator +,-,*,/,...

        [[nodiscard]] bool is_null() const { return !_value; }

    private:
        T *_value{nullptr};
    }; // class Nullable<T>
#endif


#if 0
    template<class OS = std::ostream>
    class streamer_any {
    public:
        // OS &os;
        typedef std::unordered_map<std::type_index, std::function<void(std::ostream &os, std::any const &)>> R;
        R any_visitors;

        streamer_any()
            : any_visitors{
                      to_any_visitor<void>([](std::ostream &os) { os << "{}"; }),
                      to_any_visitor<bool>([](std::ostream &os, bool x) { os << std::boolalpha << x; }),
                      to_any_visitor<int>([](std::ostream &os, int x) { os << x; }),
                      to_any_visitor<int8_t>([](std::ostream &os, int8_t x) { os << x; }),
                      to_any_visitor<int16_t>([](std::ostream &os, int16_t x) { os << x; }),
                      to_any_visitor<int32_t>([](std::ostream &os, int32_t x) { os << x; }),
                      to_any_visitor<int64_t>([](std::ostream &os, int64_t x) { os << x; }),
                      to_any_visitor<unsigned>([](std::ostream &os, unsigned x) { os << x; }),
                      to_any_visitor<uint8_t>([](std::ostream &os, uint8_t x) { os << x; }),
                      to_any_visitor<uint16_t>([](std::ostream &os, uint16_t x) { os << x; }),
                      to_any_visitor<uint32_t>([](std::ostream &os, uint32_t x) { os << x; }),
                      to_any_visitor<uint64_t>([](std::ostream &os, uint64_t x) { os << x; }),
                      to_any_visitor<long>([](std::ostream &os, long x) { os << x; }),
                      to_any_visitor<unsigned long>([](std::ostream &os, unsigned long x) { os << x; }),
                      to_any_visitor<float>([](std::ostream &os, float x) { os << x; }),
                      to_any_visitor<double>([](std::ostream &os, double x) { os << x; }),
                      to_any_visitor<char const *>([](std::ostream &os, char const *s) { os << std::quoted(s); }),
                      to_any_visitor<std::chrono::nanoseconds>([](std::ostream &os, const std::chrono::nanoseconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::microseconds>([](std::ostream &os, const std::chrono::microseconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::milliseconds>([](std::ostream &os, const std::chrono::milliseconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::seconds>([](std::ostream &os, const std::chrono::seconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::minutes>([](std::ostream &os, const std::chrono::minutes &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::hours>([](std::ostream &os, const std::chrono::hours &x) { cmdr::chrono::format_duration(os, x); }),
                      // ... add more handlers for your types ...
              } {}


        template<class T, class F>
        inline std::pair<const std::type_index, std::function<void(std::ostream &os, std::any const &)>>
        to_any_visitor(F const &f) {
            return {
                    std::type_index(typeid(T)),
                    [g = f](std::ostream &os, std::any const &a) {
                        if constexpr (std::is_void_v<T>)
                            g(os);
                        else
                            g(os, std::any_cast<T const &>(a));
                    }};
        }

        inline void process(std::ostream &os, const std::any &a) {
            if (const auto it = any_visitors.find(std::type_index(a.type()));
                it != any_visitors.cend()) {
                it->second(os, a);
            } else {
                std::cout << "Unregistered type " << std::quoted(a.type().name());
            }
        }

        template<class T, class F>
        inline void register_any_visitor(F const &f) {
            std::cout << "Register visitor for type "
                      << std::quoted(typeid(T).name()) << '\n';
            any_visitors.insert(to_any_visitor<T>(f));
        }
    };
#endif


    template<class Visitor>
    inline void visit_any_as(std::any const &, Visitor &&) {
        cmdr_throw_line("std::any contained no suitable type, unable to visit");
    }

    template<class First, class... Rest, class Visitor>
    inline void visit_any_as(std::any const &any, Visitor &&visitor) {
        First const *value = std::any_cast<First>(&any);
        if (value) {
            visitor(*value);
        } else {
            visit_any_as<Rest...>(any, std::forward<Visitor>(visitor));
        }
    }

} // namespace cmdr::util


namespace cmdr::util {

    template<typename T>
    class singleton {
    public:
        static T &instance();

        singleton(const singleton &) = delete;
        singleton &operator=(const singleton) = delete;

    protected:
        struct token {};
        singleton() = default;
    };

    template<typename T>
    inline T &singleton<T>::instance() {
        static const std::unique_ptr<T> instance{new T{token{}}};
        return *instance;
    }


    template<typename C, typename... Args>
    class singleton_with_optional_construction_args {
    private:
        singleton_with_optional_construction_args() = default;
        static C *_instance;

    public:
        ~singleton_with_optional_construction_args() {
            delete _instance;
            _instance = nullptr;
        }
        static C &instance(Args... args) {
            if (_instance == nullptr)
                _instance = new C(args...);
            return *_instance;
        }
    };

    template<typename C, typename... Args>
    C *singleton_with_optional_construction_args<C, Args...>::_instance = nullptr;

#if defined(_DEBUG) && defined(NEVER_USED)
    inline void test_singleton_with_optional_construction_args() {
        int &i = singleton_with_optional_construction_args<int, int>::instance(1);
        UTEST_CHECK(i == 1);

        tester1 &t1 = singleton_with_optional_construction_args<tester1, int>::instance(1);
        UTEST_CHECK(t1.result() == 1);

        tester2 &t2 = singleton_with_optional_construction_args<tester2, int, int>::instance(1, 2);
        UTEST_CHECK(t2.result() == 3);
    }
#endif // defined(NEVER_USED)

} // namespace cmdr::util

#include <cstdio>
#include <cstring>
#include <istream>
#include <memory>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <thread>
#if defined(OS_WIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#endif

namespace cmdr::process {

    namespace detail {
        class execbuf : public std::streambuf {
        protected:
            std::string output;
            int_type underflow() override {
                if (gptr() < egptr()) return traits_type::to_int_type(*gptr());
                return traits_type::eof();
                // return std::streambuf::underflow();
            }
            // int_type underflow(int_type character) {
            //     if (gptr() < egptr()) return traits_type::to_int_type(*gptr());
            //     return traits_type::eof();
            // }

#if defined(OS_WIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        private:
            int system_and_capture(
                    std::string cmdline,     //Command Line
                    std::string workdir,     //set to '.' for current directory
                    std::string &lst_stdout, //Return List of StdOut
                    std::string &lst_stderr, //Return List of StdErr
                    uint32_t &ret_code)      //Return Exit Code
            {
                int success;
                SECURITY_ATTRIBUTES security_attributes;
                HANDLE stdout_rd = INVALID_HANDLE_VALUE;
                HANDLE stdout_wr = INVALID_HANDLE_VALUE;
                HANDLE stderr_rd = INVALID_HANDLE_VALUE;
                HANDLE stderr_wr = INVALID_HANDLE_VALUE;
                PROCESS_INFORMATION process_info;
                STARTUPINFO startup_info;
                std::thread stdout_thread;
                std::thread stderr_thread;

                security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
                security_attributes.bInheritHandle = TRUE;
                security_attributes.lpSecurityDescriptor = nullptr;

                if (!CreatePipe(&stdout_rd, &stdout_wr, &security_attributes, 0) ||
                    !SetHandleInformation(stdout_rd, HANDLE_FLAG_INHERIT, 0)) {
                    return -1;
                }

                if (!CreatePipe(&stderr_rd, &stderr_wr, &security_attributes, 0) ||
                    !SetHandleInformation(stderr_rd, HANDLE_FLAG_INHERIT, 0)) {
                    if (stdout_rd != INVALID_HANDLE_VALUE) CloseHandle(stdout_rd);
                    if (stdout_wr != INVALID_HANDLE_VALUE) CloseHandle(stdout_wr);
                    return -2;
                }

                ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
                ZeroMemory(&startup_info, sizeof(STARTUPINFO));

                startup_info.cb = sizeof(STARTUPINFO);
                startup_info.hStdInput = 0;
                startup_info.hStdOutput = stdout_wr;
                startup_info.hStdError = stderr_wr;

                if (stdout_rd || stderr_rd)
                    startup_info.dwFlags |= STARTF_USESTDHANDLES;

                // Make a copy because CreateProcess needs to modify string buffer
                char cmdline_full[MAX_PATH];
                strncpy(cmdline_full, cmdline.c_str(), MAX_PATH);
                cmdline_full[MAX_PATH - 1] = 0;

                success = CreateProcess(
                        nullptr,
                        cmdline_full,
                        nullptr,
                        nullptr,
                        TRUE,
                        CREATE_NO_WINDOW,
                        nullptr,
                        workdir.c_str(),
                        &startup_info,
                        &process_info);
                CloseHandle(stdout_wr);
                CloseHandle(stderr_wr);

                if (!success) {
                    CloseHandle(process_info.hProcess);
                    CloseHandle(process_info.hThread);
                    CloseHandle(stdout_rd);
                    CloseHandle(stderr_rd);
                    return -4;
                } else {
                    CloseHandle(process_info.hThread);
                }

                if (stdout_rd) {
                    stdout_thread = std::thread([&]() {
                        DWORD n;
                        const size_t bufsize = 1000;
                        char buffer[bufsize];
                        for (;;) {
                            n = 0;
                            int success = ReadFile(
                                    stdout_rd,
                                    buffer,
                                    (DWORD) bufsize,
                                    &n,
                                    nullptr);
                            // cmdr_verbose_debug("STDERR: Success:%d n:%d\n", success, (int) n);
                            if (!success || n == 0)
                                break;
                            std::string s(buffer, n);
                            // cmdr_verbose_debug("STDOUT:(%s)\n", s.c_str());
                            lst_stdout += s;
                        }
                        // cmdr_verbose_debug("STDOUT:BREAK!\n");
                    });
                }

                if (stderr_rd) {
                    stderr_thread = std::thread([&]() {
                        DWORD n;
                        const size_t bufsize = 1000;
                        char buffer[bufsize];
                        for (;;) {
                            n = 0;
                            int success = ReadFile(
                                    stderr_rd,
                                    buffer,
                                    (DWORD) bufsize,
                                    &n,
                                    nullptr);
                            // cmdr_verbose_debug("STDERR: Success:%d n:%d\n", success, (int) n);
                            if (!success || n == 0)
                                break;
                            std::string s(buffer, n);
                            // cmdr_verbose_debug("STDERR:(%s)\n", s.c_str());
                            lst_stderr += s;
                        }
                        // cmdr_verbose_debug("STDERR:BREAK!\n");
                    });
                }

                WaitForSingleObject(process_info.hProcess, INFINITE);
                if (!GetExitCodeProcess(process_info.hProcess, (DWORD *) &ret_code))
                    ret_code = (uint32_t) -1;

                CloseHandle(process_info.hProcess);

                if (stdout_thread.joinable())
                    stdout_thread.join();

                if (stderr_thread.joinable())
                    stderr_thread.join();

                CloseHandle(stdout_rd);
                CloseHandle(stderr_rd);

                return 0;
            }
#endif

        public:
            execbuf(const char *command) {
#if defined(OS_WIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
                // _rc = std::system(command); // execute the UNIX command "ls -l >test.txt"
                //                            //std::cout << std::ifstream("test.txt").rdbuf();
                //                            // std::cout << "Exit code: " << WEXITSTATUS(_rc) << std::endl;
                std::string str_out, str_err;
                uint32_t rc;
                _rc = system_and_capture(command, ".", str_out, str_err, rc);
                if (_rc == 0)
                    _rc = (int) rc;

                this->output += str_out.data();
                this->output += str_err.data();
                setg((char *) this->output.data(), (char *) this->output.data(), (char *) (this->output.data() + this->output.size()));

#else // try POSIX
                std::array<char, 128> buffer;
                // std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
                std::unique_ptr<FILE, std::function<void(FILE *)>> pipe(popen(command, "r"), [this](FILE *f) {
                    _rc = pclose(f);
                    if (_rc == EXIT_SUCCESS) {        // == 0
                    } else if (_rc == EXIT_FAILURE) { // EXIT_FAILURE is not used by all programs, maybe needs some adaptation.
                    }
                });
                if (!pipe) {
                    throw std::runtime_error("popen() failed!");
                }
                while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                    this->output += buffer.data();
                }
                setg((char *) this->output.data(), (char *) this->output.data(), (char *) (this->output.data() + this->output.size()));

// auto rc = pclose(pipe);
#endif
            }

            int _rc;
        };
    } // namespace detail

    /**
     * @brief execute a shell command and capture the stdout.
     * @details For example:
     * 
     *             cmdr::process::exec dot("dot aa.dot -T png -o aa.png -v");
     *             std::cout << dot.rdbuf();
     *             
     */
    class exec : public std::istream {
    protected:
        detail::execbuf buffer;

    public:
        exec(char const *command)
            : std::istream(nullptr)
            , buffer(command) {
            this->rdbuf(&buffer);
        }

        int retcode() const { return buffer._rc; }
    };


    inline void wait_a_key(const char *headline = nullptr) {
        if (headline) std::cout << headline;
        else
            std::cout << "Press Enter to Continue...";
        std::cin.ignore();
    }

} // namespace cmdr::process

namespace cmdr::util {

    /**
     * @brief defer&lt;T&gt; provides a RAII wrapper for your lambda function.
     * @tparam T is a class which has a member function: <code>void close();</code>
     * @details For example:
     * 
     *     std::ofstream ofs("aa.dot");
     *     cmdr::util::defer ofs_closer(ofs);
     *     cmdr::util::defer ofs_closer(ofs, [](){ return; });
     *     cmdr::util::defer<bool> a_closer([](){
     *         // ... your closer here
     *     });
     *     
     * \br The alternate approach via std::unique_ptr:
     * 
     *     auto ofs = std::make_unique<std::ofstream>("aa.dot", std::ofstream::out);
     *     *ofs.get() << "1";
     *     
     */
    template<class T, class _D = std::default_delete<T>>
    class defer final {
    public:
        defer(T &c, _D const &fn = nullptr)
            : _c(c)
            , _fn(fn) {}
        defer(_D const &fn, T &c = T{})
            : _c(c)
            , _fn(fn) {}
        ~defer() {
            _c.close();
            if (_fn) { _fn(); }
        }

    private:
        T &_c;
        _D _fn;
    };

    /**
     * @brief defer provides a RAII wrapper for your lambda function.
     * @details For example:
     * 
     *     cmdr::util::defer<bool> a_closer([](){
     *         // ... your closer here
     *     });
     */
    template<>
    class defer<bool> final {
    public:
        defer(std::function<void()> const &fn, bool = false)
            : _fn(fn) {}
        ~defer() {
            if (_fn) { _fn(); }
        }

    private:
        std::function<void()> _fn;
    };

    inline std::string detect_shell_env() {
        auto *str = std::getenv("SHELL");
        if (str != nullptr) {
            auto path = std::filesystem::path(str);
            return path.filename().u8string();
        }
        return "unknown";
    }

} //namespace cmdr::util


#endif //CMDR_CXX11_CMDR_UTILS_HH
