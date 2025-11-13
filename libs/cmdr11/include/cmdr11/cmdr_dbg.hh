//
// Created by Hedzr Yeh on 2021/2/2.
//

#ifndef CMDR_CXX11_CMDR_DBG_HH
#define CMDR_CXX11_CMDR_DBG_HH

#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <typeinfo>
#include <vector>

#include <cassert>
#include <exception>
#include <stdexcept>

#include <array> // std::array
#include <string>
#include <string_view>
#include <utility> // std::index_sequence

#include "cmdr_defs.hh"

#ifndef OS_WIN
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifndef OS_WIN
#define OS_WIN 1
#endif
#else
#define OS_WIN 0
#endif
#endif

#if !OS_WIN
#include <execinfo.h>
#endif

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#include <utility>
#endif

#ifdef __GNUG__
#include <signal.h>
#include <unistd.h>
#endif


#ifndef __FUNCTION_NAME__
#ifdef __clang__
#define __FUNCTION_NAME__ __PRETTY_FUNCTION__
#elif defined(__GNUC__)
#define __FUNCTION_NAME__ __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define __FUNCTION_NAME__ __FUNCSIG__
#endif
#endif


#ifndef __PRINT_IF_FALSE
#define __PRINT_IF_FALSE
// for C++ assert:
//    assert(print_if_false(a==b, "want a equal to b"));
//
inline bool print_if_false(const bool assertion, const char *msg) {
  if (!assertion) {
    std::cerr << msg << '\n';
  }
  return assertion;
}
inline bool print_if_false(const bool assertion, const std::string &msg) { return print_if_false(assertion, msg.c_str()); }
#endif


#ifndef assertm
// assertm(a == b, "want a equal to b");
#ifdef _DEBUG
#define assertm(expr, msg) \
  __M_Assert(#expr, expr, __FILE__, __LINE__, __FUNCTION_NAME__, msg)
inline void __M_Assert(const char *expr_str, bool expr, const char *file, int line, const char *func, const char *msg) {
  if (!expr) {
    std::cerr << std::setfill(' ')
              << std::setw(19) << "Assert failed : " << msg << "\n"
              << std::setw(19) << "Expected : " << expr_str << "\n"
              << std::setw(19) << "Source : " << func << " at " << file << ':' << line << "\n";
    std::abort();
  }
}
inline void __M_Assert(const char *expr_str, bool expr,
                       const char *file, int line, const char *func,
                       const std::string &msg) { __M_Assert(expr_str, expr, file, line, func, msg.c_str()); }
#else
#define assertm(expr, msg) (void) 9
#endif
#endif

// join (constexpr)
namespace cmdr {

  template<std::string_view const &...Strings>
  struct join {
    // Join all strings into a single std::array of chars
    static constexpr auto impl() noexcept {
      constexpr std::size_t len = (Strings.size() + ... + 0);
      std::array<char, len + 1> arr1{};
      auto append = [i = 0, &arr1](auto const &s) mutable {
        for (auto c: s) arr1[i++] = c;
      };
      (append(Strings), ...);
      arr1[len] = 0;
      return arr1;
    }
    // Give the joined string static storage
    static constexpr auto arr = impl();
    // View as a std::string_view
    static constexpr std::string_view value{arr.data(), arr.size() - 1};
  };
  // Helper to get the value out
  template<std::string_view const &...Strings>
  static constexpr auto join_v = join<Strings...>::value;

} // namespace cmdr
// type_name, short_type_name, 
namespace cmdr::debug {

#if 1
  template<typename T>
  constexpr std::string_view type_name();

  template<>
  constexpr std::string_view type_name<void>() { return "void"; }

  namespace detail {

    using type_name_prober = void;

    template<typename T>
    constexpr std::string_view wrapped_type_name() {
#ifdef __clang__
      return __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
      return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
      return __FUNCSIG__;
#else
#error "Unsupported compiler"
#endif
    }

    constexpr std::size_t wrapped_type_name_prefix_length() {
      return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>());
    }

    constexpr std::size_t wrapped_type_name_suffix_length() {
      return wrapped_type_name<type_name_prober>().length() - wrapped_type_name_prefix_length() - type_name<type_name_prober>().length();
    }

    template<typename T>
    constexpr std::string_view type_name() {
      constexpr auto wrapped_name     = wrapped_type_name<T>();
      constexpr auto prefix_length    = wrapped_type_name_prefix_length();
      constexpr auto suffix_length    = wrapped_type_name_suffix_length();
      constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
      return wrapped_name.substr(prefix_length, type_name_length);
    }

  } // namespace detail

  /**
   * @brief return the literal of a datatype in constexpr.
   * @tparam T the datatype
   * @return std::string_view
   * @note the returning string is a string_view, make a copy before print it:
   *
   * 1. use std::ostream directly:
   * @code{c++}
   * std::cout << cmdr::debug::type_name&lt;std::string>() << '\n';
   * @endcode
   *
   * 2. wrap the string_view into a std::string:
   * @code{c++}
   * std::cout &lt;&lt; std::string(cmdr::debug::type_name&lt;std::string>()) &lt;&lt; '\n';
   * printf(">>> %s\n", std::string(cmdr::debug::type_name&lt;std::string>()).c_str());
   * @endcode
   *
   */
  template<typename T>
  constexpr std::string_view type_name() {
    constexpr auto r = detail::type_name<T>();

    using namespace std::string_view_literals;
    constexpr auto pr1 = "struct "sv;
    auto ps1           = r.find(pr1);
    auto st1           = (ps1 == 0 ? pr1.length() : 0);
    auto name1         = r.substr(st1);
    constexpr auto pr2 = "class "sv;
    auto ps2           = name1.find(pr2);
    auto st2           = (ps2 == 0 ? pr2.length() : 0);
    auto name2         = name1.substr(st2);
    constexpr auto pr3 = "union "sv;
    auto ps3           = name2.find(pr3);
    auto st3           = (ps3 == 0 ? pr3.length() : 0);
    auto name3         = name2.substr(st3);

    return name3;
  }

  /**
   * @brief remove the scoped prefix (before '::')
   * @tparam T
   * @return
   */
  template<typename T>
  constexpr auto short_type_name() -> std::string_view {
    constexpr auto &value = type_name<T>();
    constexpr auto end    = value.rfind("::");
    return std::string_view{value.data() + (end != std::string_view::npos ? end + 2 : 0)};
  }

  // https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c
#else

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
#ifndef _MSC_VER
    return r;
#else
    const char *const pr1 = "struct ";
    auto ps1 = r.find(pr1);
    auto st1 = (ps1 != std::string::npos ? ps1 + sizeof(pr1) - 1 : 0);
    auto name1 = r.substr(st1);
    const char *const pr2 = "class ";
    auto ps2 = name1.find(pr2);
    auto st2 = (ps2 != std::string::npos ? ps2 + sizeof(pr2) - 1 : 0);
    auto name2 = name1.substr(st2);
    const char *const pr3 = "union ";
    auto ps3 = name2.find(pr3);
    auto st3 = (ps3 != std::string::npos ? ps3 + sizeof(pr3) - 1 : 0);
    auto name3 = name2.substr(st3);
    return name3;
#endif
  }

  /**
   * @brief remove the scoped prefix (before '::')
   * @tparam T
   * @return
   */
  template<typename T>
  constexpr auto short_type_name() -> std::string_view {
    const auto value = type_name<T>();
    const auto end   = value.rfind("::");
    return std::string_view{value.data() + (end != std::string_view::npos ? end + 2 : 0)};
  }

#else
  template<typename T>
  constexpr auto type_name_1() noexcept {
    // std::string_view name, prefix, suffix;
#ifdef __clang__
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
    constexpr auto prefix   = std::string_view{"auto type_name() [T = "};
    constexpr auto suffix   = std::string_view{"]"};
#elif defined(__GNUC__)
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
    constexpr auto prefix   = std::string_view{"constexpr auto type_name() [with T = "};
    constexpr auto suffix   = std::string_view{"]"};
#elif defined(_MSC_VER)
    constexpr auto function = std::string_view{__FUNCSIG__};
    constexpr auto prefix   = std::string_view{"auto __cdecl type_name<"};
    constexpr auto suffix   = std::string_view{">(void) noexcept"};
#else
    constexpr auto function = std::string_view{"Error: unsupported compiler"};
    constexpr auto prefix   = std::string_view{""};
    constexpr auto suffix   = std::string_view{""};
#endif

    // name.remove_suffix(suffix.size());
    // name.remove_prefix(prefix.size());

    constexpr auto start    = function.find(prefix) + prefix.size();
    constexpr auto end      = function.rfind(suffix);
    return function.substr(start, (end - start));
  }

  template<std::size_t... Idxs>
  constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>) {
    return std::array{str[Idxs]..., '\n'};
  }

  template<typename T>
  constexpr auto type_name_array() {
#if defined(__clang__)
    constexpr auto prefix   = std::string_view{"[T = "};
    constexpr auto suffix   = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
    constexpr auto prefix   = std::string_view{"with T = "};
    constexpr auto suffix   = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
    constexpr auto prefix   = std::string_view{"type_name_array<"};
    constexpr auto suffix   = std::string_view{">(void)"};
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

    constexpr auto start    = function.find(prefix) + prefix.size();
    constexpr auto end      = function.rfind(suffix);

    static_assert(start < end);

    constexpr auto name = function.substr(start, (end - start));
#if !defined(_MSC_VER)
    // return substring_as_array(name, std::make_index_sequence<name.size()>{});
    return name;
#else
    constexpr auto pr1      = std::string_view{"struct "};
    constexpr auto ps1      = r.find(pr1);
    constexpr auto st1      = (ps1 >= 0 ? ps1 : -pr1.size()) + pr1.size();
    constexpr auto name1    = r.substr(st1);
    constexpr auto pr2      = std::string_view{"class "};
    constexpr auto ps2      = name1.find(pr2);
    constexpr auto st2      = (ps2 >= 0 ? ps2 : -pr2.size()) + pr2.size();
    constexpr auto name2    = name1.substr(st2);
    constexpr auto pr3      = std::string_view{"union "};
    constexpr auto ps3      = name2.find(pr3);
    constexpr auto st3      = (ps3 >= 0 ? ps3 : -pr3.size()) + pr3.size();
    constexpr auto name3    = name2.substr(st3);
    return name3;
#endif
  }

  template<typename T>
  struct type_name_holder {
    static inline constexpr auto value = type_name_array<T>();
  };

  /**
   * @brief return the literal of a datatype in constexpr.
   * @tparam T the datatype
   * @return std::string_view
   * @note the returning string is a string_view, make a copy before print it:
   *
   * 1. use std::ostream directly:
   * @code{c++}
   * std::cout << cmdr::debug::type_name&lt;std::string>() << '\n';
   * @endcode
   *
   * 2. wrap the string_view into a std::string:
   * @code{c++}
   * std::cout << std::string(cmdr::debug::type_name&lt;std::string>()) << '\n';
   * printf(">>> %s\n", std::string(cmdr::debug::type_name&lt;std::string>()).c_str());
   * @endcode
   *
   */
  template<typename T>
  constexpr auto type_name() -> std::string_view {
    constexpr auto &value = type_name_holder<T>::value;
    return std::string_view{value.data(), value.size()};
  }

  /**
   * @brief remove the scoped prefix (before '::')
   * @tparam T
   * @return
   */
  template<typename T>
  constexpr auto short_type_name() -> std::string_view {
    constexpr auto &value = type_name_holder<T>::value;
    constexpr auto end    = value.rfind("::");
    return std::string_view{value.data() + (end != std::string_view::npos ? end + 2 : 0)};
  }

  // https://bitwizeshift.github.io/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/
#endif
#endif // 1 or 0


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

} // namespace cmdr::debug

// demangle, type(T), save_stacktrace, print_stacktrace
namespace cmdr::debug {

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
  //
  /** Print a demangled stack backtrace of the caller function to FILE* out. */
  template<int max_frames = 63>
  static inline void print_stacktrace(FILE *out = stderr, const int skip = 1) {
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
    char **symbollist   = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    auto funcname      = static_cast<char *>(malloc(funcnamesize));

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
        int sp       = 0;
        begin_name   = nullptr;
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
        *begin_name++   = '\0';
        *begin_offset++ = '\0';
        *end_offset     = '\0';

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
    auto funcname      = static_cast<char *>(malloc(funcnamesize));
    for (auto const &str: st) {
      char *begin_name = nullptr, *begin_offset = nullptr, *end_offset = nullptr;
      const auto data = const_cast<char *>(str.c_str());

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
        int sp       = 0;
        begin_name   = nullptr;
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
        *begin_name++   = '\0';
        *begin_offset++ = '\0';
        *end_offset     = '\0';

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


//
//
//

namespace cmdr::opt {
  class arg;
  class cmd;
} // namespace cmdr::opt
// cmdr_throw_line(arg), cmdr_throw_as(typ, ...), cmdr_exception, cmdr_biz_error/required_flag_missed/dup_error
namespace cmdr::exception {

  class cmdr_exception : public std::runtime_error {
    std::vector<std::string> st; // serialized stacktrace frames here

  protected:
    std::string msg;

  public:
    cmdr_exception(const char *file, const int line, const std::string &arg)
        : std::runtime_error(arg) {
      std::ostringstream o;
      o << arg << "  " << file << ":" << line;
      msg = o.str();
#if !OS_WIN
      auto v = cmdr::debug::save_stacktrace(2);
      st.swap(v);
#endif
    }
    ~cmdr_exception() noexcept override = default;
    [[nodiscard]] const char *what() const noexcept override {
      return msg.c_str();
    }
    [[nodiscard]] std::vector<std::string> const &stacktrace() const { return st; }
  };

#define cmdr_throw_line(arg) throw cmdr::exception::cmdr_exception(__FILE__, __LINE__, arg)

  class cmdr_biz_error : public cmdr_exception {
  public:
    using cmdr_exception::cmdr_exception;
  };

  class required_flag_missed : public cmdr_biz_error {
  public:
    using cmdr_biz_error::cmdr_biz_error;
  };

  class dup_error : public cmdr_biz_error {
  public:
    using cmdr_biz_error::cmdr_biz_error;
  };

#define cmdr_throw_as(typ, ...) throw cmdr::exception::typ(__FILE__, __LINE__, __VA_ARGS__)

} // namespace cmdr::exception

//

// dump_stacktrace, CMDR_DUMP_STACK_TRACE/CMDR_DUMP_WITHOUT_STACK_TRACE
namespace cmdr::debug {
  // unwrap nested exceptions, printing each nested exception to std::cerr.
  inline void dump_stacktrace(
      const char *file,
      int line,
      std::exception const &e,
      bool print_stack  = true,
      std::size_t depth = 0) {
    std::cerr << "      [EX] [ERR] : " // << demangle(typeid(e).name()) << ", "
              << std::string(depth, ' ') << e.what();
    if (line > 0) std::cerr << ' ' << ' ' << file << ':' << line;
    std::cerr << '\n';
    if (print_stack) {
      if (depth == 0 && "cmdr::exception::cmdr_exception" == type(e)) {
#if !OS_WIN
        print_stacktrace(static_cast<cmdr::exception::cmdr_exception const *>(&e)->stacktrace());
#endif
      }
      try {
        std::rethrow_if_nested(e);
      } catch (const std::exception &nested) {
        dump_stacktrace(file, line, nested, print_stack, depth + 1);
      }
    }
#if defined(__GNUC__) || defined(__clang__)
    if (depth == 0) {
      std::cerr << "    - [CAPTURED] : " << file << ":" << line << '\n';
      if (print_stack) {
        print_stacktrace();
      }
    }
#endif
  }
#define CMDR_DUMP_STACK_TRACE(e) cmdr::debug::dump_stacktrace(__FILE__, __LINE__, e)
#define CMDR_DUMP_WITHOUT_STACK_TRACE(e) cmdr::debug::dump_stacktrace(__FILE__, __LINE__, e, false)

} // namespace cmdr::debug

//
// SIGSEGV handler
//

// UnhandledExceptionHookInstaller, SigSegVInstaller, SignalInstaller
#if !OS_WIN
namespace cmdr::debug {

  /**
   * @brief
   * @todo add supports to windows SEH
   */
  class UnhandledExceptionHookInstaller final {
    std::function<void()> _f{};
    static UnhandledExceptionHookInstaller *_this;

  public:
    UnhandledExceptionHookInstaller() {
      std::set_terminate(handler);
      _this = this;
    }
    [[maybe_unused]] explicit UnhandledExceptionHookInstaller(std::function<void()> f)
        : _f(std::move(f)) {
      std::set_terminate(handler);
      _this = this;
    }
    ~UnhandledExceptionHookInstaller() = default;

    static void handler() {
#if 1
      fprintf(stderr, "Unhandled Exception: ...\n");
      void *trace_elems[20];
      const int trace_elem_count(backtrace(trace_elems, 20));
      char **stack_syms(backtrace_symbols(trace_elems, trace_elem_count));
      for (int i = 0; i < trace_elem_count; ++i) {
        std::cout << stack_syms[i] << "\n";
      }
      free(stack_syms);
#else
      print_stacktrace(stderr, 0);
#endif
      if (_this->_f)
        _this->_f();
      exit(1);
    }
  };

  inline UnhandledExceptionHookInstaller *UnhandledExceptionHookInstaller::_this{nullptr};


  /**
   * @brief a SIGSEGV handler Installer once only. Simple and not thread-safe but it's not point, right? You're crashing here after all.
   */
  class SigSegVInstaller final {
    std::function<void(int sig)> _f{};
    static SigSegVInstaller *_this;

  public:
    SigSegVInstaller() {
      signal(SIGSEGV, handler);
      _this = this;
    }
    [[maybe_unused]] explicit SigSegVInstaller(std::function<void(int sig)> f)
        : _f(std::move(f)) {
      signal(SIGSEGV, handler);
      _this = this;
    }
    ~SigSegVInstaller() = default;

    static void handler(int sig) {
#if 0
            void *array[10];
            size_t size;

            // get void*'s for all entries on the stack
            size = backtrace(array, 10);

            // print out all the frames to stderr
            fprintf(stderr, "Error: signal %d:\n", sig);
            backtrace_symbols_fd(array, size, STDERR_FILENO);
#else
      fprintf(stderr, "Error: signal %d:\n", sig);
      print_stacktrace(stderr, 0);
      if (_this->_f)
        _this->_f(sig);
#endif
      exit(1);
    }

    [[maybe_unused]] static void baz() {
      const int *foo = reinterpret_cast<int *>(-1); // make a bad pointer
      printf("%d\n", *foo);  // causes segfault
    }
  }; // class SigSegVInstaller

  inline SigSegVInstaller *SigSegVInstaller::_this{nullptr};

  /**
   * @brief a SIGXXX handler Installer once only. Simple and not thread-safe but it's not point, right? You're crashing here after all.
   */
  template<int signal_catching = SIGBUS>
  class SignalInstaller final {
    std::function<void(int sig)> _f{};
    static SignalInstaller *_this;

  public:
    SignalInstaller() {
      signal(signal_catching, handler);
      _this = this;
    }
    [[maybe_unused]] explicit SignalInstaller(std::function<void(int sig)> f)
        : _f(std::move(f)) {
      signal(signal_catching, handler);
      _this = this;
    }
    ~SignalInstaller() = default;

    static void handler(int sig_) {
#if 0
                    void *array[10];
                    size_t size;

                    // get void*'s for all entries on the stack
                    size = backtrace(array, 10);

                    // print out all the frames to stderr
                    fprintf(stderr, "Error: signal %d:\n", sig_);
                    backtrace_symbols_fd(array, size, STDERR_FILENO);
#else
      fprintf(stderr, "Error: signal %d:\n", sig_);
      print_stacktrace(stderr, 0);
      if (_this->_f)
        _this->_f(sig_);
#endif
      exit(1);
    }

    [[maybe_unused]] static void baz() {
      const int *foo = reinterpret_cast<int *>(-1); // make a bad pointer
      printf("%d\n", *foo);  // causes segfault
    }
  }; // class SignalInstaller

  template<int signal_catching>
  inline SignalInstaller<signal_catching> *SignalInstaller<signal_catching>::_this{nullptr};

} // namespace cmdr::debug
#endif // !OS_WIN

#endif // CMDR_CXX11_CMDR_DBG_HH
