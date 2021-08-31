//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_DEFS_HH
#define CMDR_CXX11_CMDR_DEFS_HH


#include <initializer_list>
#include <iostream>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

#include <filesystem>

#include <cstddef>
#include <cstdint>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
// #define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#else
#include <unistd.h>
#endif

#if !defined(_DEBUG) && defined(DEBUG)
#define _DEBUG DEBUG
#endif

#ifndef _UNUSED_DEFINED
#define _UNUSED_DEFINED
#ifdef __clang__

//#ifndef UNUSED
//#define UNUSED(...) [__VA_ARGS__](){}
//#endif
template<typename... Args>
inline void UNUSED([[maybe_unused]] Args &&...args) {
    (void) (sizeof...(args));
}

#elif __GNUC__ || _MSC_VER

// c way unused
#ifndef UNUSED
#define UNUSED0()
#define UNUSED1(a) (void) (a)
#define UNUSED2(a, b) (void) (a), UNUSED1(b)
#define UNUSED3(a, b, c) (void) (a), UNUSED2(b, c)
#define UNUSED4(a, b, c, d) (void) (a), UNUSED3(b, c, d)
#define UNUSED5(a, b, c, d, e) (void) (a), UNUSED4(b, c, d, e)

#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(100, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)

#define ALL_UNUSED_IMPL_(nargs) UNUSED##nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define UNUSED(...)                           \
    ALL_UNUSED_IMPL(VA_NUM_ARGS(__VA_ARGS__)) \
    (__VA_ARGS__)
#endif

#endif
#endif //_UNUSED_DEFINED


#ifndef CMDR_ASSERT
#if defined(ENABLE_ASSERTS)
#define CMDR_ASSERT(...) assert(__VA_ARGS__)
#else
#define CMDR_ASSERT(...) UNUSED(__VA_ARGS__)
#endif
#endif


#ifndef DEBUG_ONLY
#if defined(_DEBUG)
#define DEBUG_ONLY(...) __VA_ARGS__
#else
#define DEBUG_ONLY(...) /* __VA_ARGS__ */
#endif
#endif

#ifndef RELEASE_ONLY
#if !defined(_DEBUG)
#define RELEASE_ONLY(...) __VA_ARGS__
#else
#define RELEASE_ONLY(...) /* __VA_ARGS__ */
#endif
#endif


#ifndef ASSERTIONS_ONLY
#if defined(CMDR_ENABLE_ASSERTIONS) && defined(_DEBUG)
#define ASSERTIONS_ONLY(...) __VA_ARGS__
#else
#define ASSERTIONS_ONLY(...) /* __VA_ARGS__ */
#endif
#endif

#ifndef NO_ASSERTIONS_ONLY
#if defined(CMDR_ENABLE_ASSERTIONS)
#define NO_ASSERTIONS_ONLY(...) /* __VA_ARGS__ */
#else
#define NO_ASSERTIONS_ONLY(...) __VA_ARGS__
#endif
#endif


#ifndef __COPY
#define __COPY(m) this->m = o.m
#endif

#if !defined(_WIN32)
#ifndef TEXT
#define TEXT(x) (#x)
#endif
#endif

#ifndef DISABLE_MSVC_WARNINGS
#if defined(_MSC_VER)
#define DISABLE_MSVC_WARNINGS(...)   \
    __pragma(warning(push))          \
            __pragma(warning(disable \
                             : __VA_ARGS__)) /*disable _ctlState prefast warning*/
#define RESTORE_MSVC_WARNINGS \
    __pragma(warning(pop))
#else
#define DISABLE_MSVC_WARNINGS(...) /* __VA_ARGS__ */
#define RESTORE_MSVC_WARNINGS
#endif
#endif

#ifndef DISABLE_WARNINGS
#if defined(_MSC_VER)
#define DISABLE_WARNINGS __pragma(warning(push, 1))
#define RESTORE_WARNINGS __pragma(warning(pop))
#else
#define DISABLE_WARNINGS           \
    _Pragma("GCC diagnostic push") \
            _Pragma("GCC diagnostic ignored \"-Wall\"")
#define RESTORE_WARNINGS \
    _Pragma("GCC diagnostic pop")
#endif
#endif

#ifndef DISABLE_UNUSED_WARNINGS
#if defined(_MSC_VER)
#define DISABLE_UNUSED_WARNINGS \
    __pragma(warning(push))     \
            __pragma(warning(disable : 4100 4101 4102))
#define RESTORE_UNUSED_WARNINGS __pragma(warning(pop))
#else
#define DISABLE_UNUSED_WARNINGS                                                             \
    _Pragma("GCC diagnostic push")                                                          \
            _Pragma("GCC diagnostic ignored \"-Wunused\"")                                  \
                    _Pragma("GCC diagnostic ignored \"-Wunused-label\"")                    \
                            _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")        \
                                    _Pragma("GCC diagnostic ignored \"-Wunused-variable\"") \
                                            _Pragma("GCC diagnostic ignored \"-Wunused-value\"")
#define RESTORE_UNUSED_WARNINGS \
    _Pragma("GCC diagnostic pop")
#endif
#endif

#ifndef DISABLE_ALIGN_WARNINGS // structure was padded due to alignment specifier
#if defined(_MSC_VER)
#define DISABLE_ALIGN_WARNINGS \
    __pragma(warning(push))    \
            __pragma(warning(disable : 4324))
#define RESTORE_ALIGN_WARNINGS __pragma(warning(pop))
#else
#define DISABLE_ALIGN_WARNINGS
#define RESTORE_ALIGN_WARNINGS
#endif
#endif


//


#ifndef CLAZZ_NON_COPYABLE
#define CLAZZ_NON_COPYABLE(clz)           \
    clz(const clz &) = delete;            \
    clz(clz &&) noexcept = delete;        \
    clz &operator=(const clz &) = delete; \
    clz &operator=(clz &&) noexcept = delete
#endif

#ifndef CLAZZ_NON_MOVEABLE
#define CLAZZ_NON_MOVEABLE(clz)    \
    clz(clz &&) noexcept = delete; \
    clz &operator=(clz &&) noexcept = delete
#endif


//


/**
 * @brief declare enum class with its string literals.
 * @details For examples:
 * @code{c++}
 *  AWESOME_MAKE_ENUM(Animal,
 *                    DOG,
 *                    CAT,
 *                    HORSE);
 *  auto dog = Animal::DOG;
 *  std::cout &lt;&lt; dog;
 *  
 *  AWESOME_MAKE_ENUM(Week,
 *                    Sunday, Monday,
 *                    Tuesday, Wednesday, Thursday, Friday, Saturday);
 *  std::cout &lt;&lt; Week::Saturday << '\n';
 * @endcode
 */
#define AWESOME_MAKE_ENUM(name, ...)                                \
    enum class name { __VA_ARGS__,                                  \
                      __COUNT };                                    \
    inline std::ostream &operator<<(std::ostream &os, name value) { \
        std::string enumName = #name;                               \
        std::string str = #__VA_ARGS__;                             \
        int len = str.length(), val = -1;                           \
        std::map<int, std::string> maps;                            \
        std::ostringstream temp;                                    \
        for (int i = 0; i < len; i++) {                             \
            if (isspace(str[i])) continue;                          \
            if (str[i] == ',') {                                    \
                std::string s0 = temp.str();                        \
                auto ix = s0.find('=');                             \
                if (ix != std::string::npos) {                      \
                    auto s2 = s0.substr(ix + 1);                    \
                    s0 = s0.substr(0, ix);                          \
                    std::stringstream ss(s2);                       \
                    ss >> val;                                      \
                } else                                              \
                    val++;                                          \
                maps.emplace(val, s0);                              \
                temp.str(std::string());                            \
            } else                                                  \
                temp << str[i];                                     \
        }                                                           \
        std::string s0 = temp.str();                                \
        auto ix = s0.find('=');                                     \
        if (ix != std::string::npos) {                              \
            auto s2 = s0.substr(ix + 1);                            \
            s0 = s0.substr(0, ix);                                  \
            std::stringstream ss(s2);                               \
            ss >> val;                                              \
        } else                                                      \
            val++;                                                  \
        maps.emplace(val, s0);                                      \
        os << enumName << "::" << maps[(int) value];                \
        return os;                                                  \
    }


//


#ifndef HAS_STRING_VIEW
#if __cplusplus >= 201703 || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define HAS_STRING_VIEW 1
#else
#define HAS_STRING_VIEW 0
#endif
#endif // HAS_STRING_VIEW

#ifndef HAS_UNCAUGHT_EXCEPTIONS
#if __cplusplus >= 201703 || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define HAS_UNCAUGHT_EXCEPTIONS 1
#else
#define HAS_UNCAUGHT_EXCEPTIONS 0
#endif
#endif // HAS_UNCAUGHT_EXCEPTIONS

#ifndef HAS_VOID_T
#if __cplusplus >= 201703 || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define HAS_VOID_T 1
#else
#define HAS_VOID_T 0
#endif
#endif // HAS_VOID_T

#ifndef ONLY_C_LOCALE
#define ONLY_C_LOCALE 0
#endif

#if defined(_MSC_VER) && (!defined(__clang__) || (_MSC_VER < 1910))
// MSVC
#ifndef _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING
#define _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING
#endif
#if _MSC_VER < 1910
//   before VS2017
#define CONSTDATA const
#define CONSTCD11
#define CONSTCD14
#define NOEXCEPT _NOEXCEPT
#else
//   VS2017 and later
#define CONSTDATA constexpr const
#define CONSTCD11 constexpr
#define CONSTCD14 constexpr
#define NOEXCEPT noexcept
#endif

#elif defined(__SUNPRO_CC) && __SUNPRO_CC <= 0x5150
// Oracle Developer Studio 12.6 and earlier
#define CONSTDATA constexpr const
#define CONSTCD11 constexpr
#define CONSTCD14
#define NOEXCEPT noexcept

#elif __cplusplus >= 201402
// C++14
#define CONSTDATA constexpr const
#define CONSTCD11 constexpr
#define CONSTCD14 constexpr
#define NOEXCEPT noexcept
#else
// C++11
#define CONSTDATA constexpr const
#define CONSTCD11 constexpr
#define CONSTCD14
#define NOEXCEPT noexcept
#endif


//


// #ifndef __has_feature
// #define __has_feature(x) 0 // Compatibility with non-clang compilers.
// #endif
// // Any compiler claiming C++11 supports, Visual C++ 2015 and Clang version supporting constexpr
// #if __cplusplus >= 201103L || _MSC_VER >= 1900 || __has_feature(cxx_constexpr) // C++ 11 implementation
// namespace detail {
//     template<typename T, std::size_t N>
//     constexpr std::size_t countof(T const (&)[N]) noexcept {
//         return N;
//     }
// } // namespace detail
// #define countof(arr) detail::countof(arr)
// #elif _MSC_VER // Visual C++ fallback
// #define countof(arr) _countof(arr)
// #elif __cplusplus >= 199711L && ( // C++ 98 trick
// defined(__INTEL_COMPILER) || defined(__clang__) ||
//         (defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))) template<typename T, std::size_t N>
//         char (&COUNTOF_REQUIRES_ARRAY_ARGUMENT(T (&)[N]))[N];
// #define countof(x) sizeof(COUNTOF_REQUIRES_ARRAY_ARGUMENT(x))
// #else
// #define countof(arr) sizeof(arr) / sizeof(arr[0])
// #endif
//
// // template<class C>
// // std::size_t countof(C const &c) {
// //     return c.size();
// // }

// char arrname[5];
// size_t count = std::extent< decltype( arrname ) >::value;
//
// char arrname[5];
// size_t count = countof( arrname );
//
// char arrtwo[5][6];
// size_t count_fst_dim = countof( arrtwo );    // 5
// size_t count_snd_dim = countof( arrtwo[0] ); // 6
//
template<typename T, size_t N>
[[maybe_unused]] inline size_t countof(T (&arr)[N]) {
    UNUSED(arr);
    return std::extent<T[N]>::value;
}


// PORTABILITY MACROS --------------------------------

#if defined(__GNUC__)
#define CMDR_EXPORT __attribute__((__visibility__("default")))
#else
#define CMDR_EXPORT
#endif
#if defined(__has_feature)
#define CMDR_HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
#define CMDR_HAS_FEATURE(...) 0
#endif
#if CMDR_HAS_FEATURE(thread_sanitizer) || __SANITIZE_THREAD__
#ifndef CMDR_SANITIZE_THREAD
#define CMDR_SANITIZE_THREAD 1
#endif
#endif

#ifndef _OS_MACROS
#define _OS_MACROS

// https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor/46177613
// https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive/8249232
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//define something for Windows (32-bit and 64-bit, this part is common)
//#define OS_WINDOWS
#define OS_WIN
#ifdef _WIN64
//define something for Windows (64-bit only)
#define OS_WIN64
#else
//define something for Windows (32-bit only)
#define OS_WIN32
#endif

#elif __APPLE__
#define OS_MAC
#define OS_MACOS
#define OS_APPLE
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
// #error "Unknown Apple platform"
#define TARGET_UNKNOWN
#endif

#elif __ANDROID__
#define OS_ANDROID

#elif __linux__
// linux
#define OS_LINUX

#elif __FreeBSD__
#define OS_FREEBSD

#elif __unix__ // all unices not caught above
// Unix
#define OS_UNIX

#elif defined(_POSIX_VERSION)
// POSIX
#define OS_OTHERS_POSIX

#else
//#error "Unknown compiler"
#define OS_UNKNOWN
#endif
#endif //_OS_MACROS

#ifdef OS_UNIX
#undef OS_UNIX
#define OS_UNIX 1
#else
#define OS_UNIX 0
#endif

#ifdef OS_FREEBSD
#undef OS_FREEBSD
#define OS_FREEBSD 1
#else
#define OS_FREEBSD 0
#endif

#ifdef OS_LINUX
#undef OS_LINUX
#define OS_LINUX 1
#else
#define OS_LINUX 0
#endif

#ifdef OS_APPLE
#undef OS_APPLE
#define OS_APPLE 1
#else
#define OS_APPLE 0
#endif

#ifdef OS_WIN
#undef OS_WIN
#define OS_WIN 1
#else
#define OS_WIN 0
#endif

#if defined(_POSIX_VERSION)
#define OS_POSIX 1
#else
#define OS_POSIX 0
#endif

#if defined(__arm__)
#define ARCH_ARM 1
#else
#define ARCH_ARM 0
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_X64 1
#else
#define ARCH_X64 0
#endif

#if defined(__aarch64__)
#define ARCH_AARCH64 1
#else
#define ARCH_AARCH64 0
#endif

#if defined(__powerpc64__)
#define ARCH_PPC64 1
#else
#define ARCH_PPC64 0
#endif

#if defined(__has_builtin)
#define HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define HAS_BUILTIN(...) 0
#endif
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(nodiscard)
#define HAS_NODISCARD [[nodiscard]]
#endif
#endif

#if !defined HAS_NODISCARD
#if defined(_MSC_VER) && (_MSC_VER >= 1700)
#define HAS_NODISCARD _Check_return_
#elif defined(__GNUC__)
#define HAS_NODISCARD __attribute__((__warn_unused_result__))
#else
#define HAS_NODISCARD
#endif
#endif

namespace cmdr::cross {
    constexpr bool kIsArchArm = ARCH_ARM == 1;
    constexpr bool kIsArchAmd64 = ARCH_X64 == 1;
    constexpr bool kIsArchAArch64 = ARCH_AARCH64 == 1;
    constexpr bool kIsArchPPC64 = ARCH_PPC64 == 1;
} // namespace cmdr::cross
namespace cmdr::cross {
#ifdef NDEBUG
    constexpr auto kIsDebug = false;
#else
    constexpr auto kIsDebug = true;
#endif
} // namespace cmdr::cross
namespace cmdr::cross {
#if defined(_MSC_VER)
    constexpr bool kIsMsvc = true;
#else
    constexpr bool kIsMsvc = false;
#endif
} // namespace cmdr::cross
namespace cmdr::cross {
#if FOLLY_SANITIZE_THREAD
    constexpr bool kIsSanitizeThread = true;
#else
    constexpr bool kIsSanitizeThread = false;
#endif
} // namespace cmdr::cross
namespace cmdr::cross {
#if defined(__linux__) && !FOLLY_MOBILE
    constexpr auto kIsLinux = true;
#else
    constexpr auto kIsLinux = false;
#endif
} // namespace cmdr::cross

//


template<typename T>
struct always_false : std::false_type {};

template<typename T>
[[maybe_unused]] constexpr bool always_false_v = always_false<T>::value;

namespace cmdr::cross {
    template<typename T>
    constexpr T constexpr_max(T a) {
        return a;
    }
    template<typename T, typename... Ts>
    constexpr T constexpr_max(T a, T b, Ts... ts) {
        return b < a ? constexpr_max(a, ts...) : constexpr_max(b, ts...);
    }
    namespace detail {
        template<typename T>
        constexpr T constexpr_log2_(T a, T e) {
            return e == T(1) ? a : constexpr_log2_(a + T(1), e / T(2));
        }
        template<typename T>
        constexpr T constexpr_log2_ceil_(T l2, T t) {
            return l2 + T(T(1) << l2 < t ? 1 : 0);
        }
        template<typename T>
        constexpr T constexpr_square_(T t) {
            return t * t;
        }
    } // namespace detail
    template<typename T>
    constexpr T constexpr_log2(T t) {
        return detail::constexpr_log2_(T(0), t);
    }
    template<typename T>
    constexpr T constexpr_log2_ceil(T t) {
        return detail::constexpr_log2_ceil_(constexpr_log2(t), t);
    }
} // namespace cmdr::cross

#ifndef _CONST_CHARS_DEFINED
#define _CONST_CHARS_DEFINED
typedef const char *const_chars;
#endif
#ifndef _STRING_ARRAY_DEFINED
#define _STRING_ARRAY_DEFINED
typedef std::vector<std::string> string_array;
#endif


#ifndef _VECTOR_TO_STRING_HELPERS_DEFINED
#define _VECTOR_TO_STRING_HELPERS_DEFINED

// namespace cmdr::detail {
//     using std::begin;
//
//     template<class T>
//     inline auto check() -> decltype(begin(std::declval<T>())) {}
// }
//
// template<class T, class = decltype(cmdr::detail::check<T>())>
// inline constexpr bool is_iterable(int) { return true; }
//
// template<class>
// inline constexpr bool is_iterable(unsigned) { return false; }

#if OS_WIN
template<typename T, typename = void>
struct is_iterable : std::false_type {};

template<typename T>
struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()),
                                  decltype(std::declval<T>().end())>>
    : std::true_type {};

template<class Container,
         std::enable_if_t<
                 is_iterable<Container>::value &&
                         !std::is_same<Container, std::string>::value &&
                         !std::is_same<Container, std::filesystem::path>::value,
                 int> = 0>
inline std::string vector_to_string(Container const &vec) {
    std::ostringstream os;
    os << '[';
    int ix = 0;
    for (auto const &v : vec) {
        if (ix++ > 0) os << ',';
        os << v;
    }
    return os.str();
}

template<class Container,
         std::enable_if_t<
                 is_iterable<Container>::value &&
                         !std::is_same<Container, std::string>::value &&
                         !std::is_same<Container, std::filesystem::path>::value,
                 int> = 0>
inline std::ostream &operator<<(std::ostream &os, Container const &o) {
    os << vector_to_string(o);
    return os;
}
#else
template<typename T, typename = void>
struct is_iterable : std::false_type {};

template<typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                  decltype(std::end(std::declval<T>()))>> : std::true_type {};

template<typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template<class TX,
         template<typename, typename...> class Container = std::vector,
         std::enable_if_t<is_iterable<Container<TX>>::value &&
                                  !std::is_same<std::decay_t<Container<TX>>, std::string>::value,
                          int> = 0>
inline std::string vector_to_string(Container<TX> const &vec) {
    std::ostringstream os;
    os << '[';
    int ix = 0;
    for (auto const &v : vec) {
        if (ix++ > 0) os << ',';
        os << v;
    }
    return os.str();
}

template<class TX,
         template<typename, typename...> class Container = std::vector,
         std::enable_if_t<is_iterable<Container<TX>>::value &&
                                  !std::is_same<std::decay_t<Container<TX>>, std::string>::value,
                          int> = 0>
inline std::ostream &operator<<(std::ostream &os, Container<TX> &o) {
    os << vector_to_string(o);
    return os;
}
#endif //OS_WIN
#endif //_VECTOR_TO_STRING_HELPERS_DEFINED


const char *const DEFAULT_KEY_PREFIX = "app";
const char *const DEFAULT_CLI_KEY_PREFIX = "app.cli";


const char *const UNSORTED_GROUP = "1230.Unsorted";
const char *const SYS_MGMT_GROUP = "9000.System Management";
// nobody group: 3333 .. 3333+50
// When you specify a group without leading sorter piece (just
// like '1230.'), we name it as nobody group.
// We sort those all nobody group with an auto-increasing
// number which have a base from 3333.
const char *const NOBODY_GROUP_SORTER = "3333";

// template<class T=std::string>
// constexpr T UNSORTED_GROUP = T("1230.Unsorted");
//
// template<class T=std::string>
// constexpr T SYS_MGMT_GROUP = T("9000.System Management");

// #if defined(CMDR_ENABLE_VERBOSE_LOG)
// #include <spdlog/spdlog.h>
// #endif


////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#if OS_WIN
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
// #define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#undef min
#undef max
#include <time.h>
namespace cmdr::cross {
    inline void setenv(const char *__name, const char *__value, int __overwrite = 1) {
        UNUSED(__overwrite);
        std::ostringstream os;
        os << __name << '=' << __value;
        (void) _putenv(os.str().c_str());
    }

    inline time_t time(time_t *_t = nullptr) {
        return ::time(_t);
    }
    // BEWRAE: this is a thread-unsafe routine, it's just for the simple scene.
    inline struct tm *gmtime(time_t const *_t = nullptr) {
        static struct tm _tm {};
        if (!_t) {
            time_t vt = time();
            gmtime_s(&_tm, &vt);
        } else
            gmtime_s(&_tm, _t);
        return &_tm;
    }

    template<class T>
    inline T max(T a, T b) { return a < b ? b : a; }
    template<class T>
    inline T min(T a, T b) { return a < b ? a : b; }
} // namespace cmdr::cross
#else
#include <algorithm>
#include <ctime>
#include <time.h>
namespace cmdr::cross {
    inline void setenv(const char *__name, const char *__value, int __overwrite = 1) {
        ::setenv(__name, __value, __overwrite);
    }

    inline time_t time(time_t *_t = nullptr) {
        return std::time(_t);
    }
    inline struct tm *gmtime(time_t const *_t = nullptr) {
        if (!_t) {
            time_t vt = time();
            return std::gmtime(&vt);
        }
        return std::gmtime(_t);
    }

    template<class T>
    inline T max(T a, T b) { return std::max(a, b); }
    template<class T>
    inline T min(T a, T b) { return std::min(a, b); }
} // namespace cmdr::cross
#endif


// Work around bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56019
#ifdef __GNUC__
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9)
namespace std {
    using ::max_align_t;
}
#endif
#endif
namespace cmdr::cross {
    //  has_extended_alignment
    //
    //  True if it may be presumed that the platform has static extended alignment;
    //  false if it may not be so presumed, even when the platform might actually
    //  have it. Static extended alignment refers to extended alignment of objects
    //  with automatic, static, or thread storage. Whether the there is support for
    //  dynamic extended alignment is a property of the allocator which is used for
    //  each given dynamic allocation.
    //
    //  Currently, very heuristical - only non-mobile 64-bit linux gets the extended
    //  alignment treatment. Theoretically, this could be tuned better.
    constexpr bool has_extended_alignment =
            kIsLinux && sizeof(void *) >= sizeof(std::uint64_t);
    namespace detail {
        // Implemented this way because of a bug in Clang for ARMv7, which gives the
        // wrong result for `alignof` a `union` with a field of each scalar type.
        // Modified for RocksDB to use C++11 only
        constexpr std::size_t max_align_v = constexpr_max(
                alignof(long double),
                alignof(double),
                alignof(float),
                alignof(long long int),
                alignof(long int),
                alignof(int),
                alignof(short int),
                alignof(bool),
                alignof(char),
                alignof(char16_t),
                alignof(char32_t),
                alignof(wchar_t),
                alignof(void *),
                alignof(std::max_align_t));
    } // namespace detail

    // max_align_v is the alignment of max_align_t.
    //
    // max_align_t is a type which is aligned at least as strictly as the
    // most-aligned basic type (see the specification of std::max_align_t). This
    // implementation exists because 32-bit iOS platforms have a broken
    // std::max_align_t (see below).
    //
    // You should refer to this as `::folly::max_align_t` in portable code, even if
    // you have `using namespace folly;` because C11 defines a global namespace
    // `max_align_t` type.
    //
    // To be certain, we consider every non-void fundamental type specified by the
    // standard. On most platforms `long double` would be enough, but iOS 32-bit
    // has an 8-byte aligned `double` and `long long int` and a 4-byte aligned
    // `long double`.
    //
    // So far we've covered locals and other non-allocated storage, but we also need
    // confidence that allocated storage from `malloc`, `new`, etc will also be
    // suitable for objects with this alignment requirement.
    //
    // Apple document that their implementation of malloc will issue 16-byte
    // granularity chunks for small allocations (large allocations are page-size
    // granularity and page-aligned). We think that allocated storage will be
    // suitable for these objects based on the following assumptions:
    //
    // 1. 16-byte granularity also means 16-byte aligned.
    // 2. `new` and other allocators follow the `malloc` rules.
    //
    // We also have some anecdotal evidence: we don't see lots of misaligned-storage
    // crashes on 32-bit iOS apps that use `double`.
    //
    // Apple's allocation reference: http://bit.ly/malloc-small
    constexpr std::size_t max_align_v = detail::max_align_v;
    DISABLE_MSVC_WARNINGS(4324) // structure was padded due to alignment specifier
    struct alignas(max_align_v) max_align_t {};
    RESTORE_MSVC_WARNINGS
    //  Memory locations within the same cache line are subject to destructive
    //  interference, also known as false sharing, which is when concurrent
    //  accesses to these different memory locations from different cores, where at
    //  least one of the concurrent accesses is or involves a store operation,
    //  induce contention and harm performance.
    //
    //  Microbenchmarks indicate that pairs of cache lines also see destructive
    //  interference under heavy use of atomic operations, as observed for atomic
    //  increment on Sandy Bridge.
    //
    //  We assume a cache line size of 64, so we use a cache line pair size of 128
    //  to avoid destructive interference.
    //
    //  mimic: std::hardware_destructive_interference_size, C++17
    constexpr std::size_t hardware_destructive_interference_size =
            kIsArchArm ? 64 : 128;
    static_assert(hardware_destructive_interference_size >= max_align_v, "math?");
    //  Memory locations within the same cache line are subject to constructive
    //  interference, also known as true sharing, which is when accesses to some
    //  memory locations induce all memory locations within the same cache line to
    //  be cached, benefiting subsequent accesses to different memory locations
    //  within the same cache line and heping performance.
    //
    //  mimic: std::hardware_constructive_interference_size, C++17
    constexpr std::size_t hardware_constructive_interference_size = 64;
    static_assert(hardware_constructive_interference_size >= max_align_v, "math?");
    //  A value corresponding to hardware_constructive_interference_size but which
    //  may be used with alignas, since hardware_constructive_interference_size may
    //  be too large on some platforms to be used with alignas.
    constexpr std::size_t cacheline_align_v = has_extended_alignment
                                                      ? hardware_constructive_interference_size
                                                      : max_align_v;
    DISABLE_MSVC_WARNINGS(4324) // structure was padded due to alignment specifier
    struct alignas(cacheline_align_v) cacheline_align_t {};
    RESTORE_MSVC_WARNINGS


    inline constexpr std::size_t cache_line_size() {
#ifdef KNOWN_L1_CACHE_LINE_SIZE
        return KNOWN_L1_CACHE_LINE_SIZE;
#else
        return hardware_destructive_interference_size;
#endif
    }

    // struct alignas(hardware_constructive_interference_size)
    //         OneCacheLiner { // 占据一条缓存线
    //     std::atomic_uint64_t x{};
    //     std::atomic_uint64_t y{};
    // } oneCacheLiner;
    //
    // struct TwoCacheLiner { // 占据二条缓存线
    //     alignas(hardware_destructive_interference_size) std::atomic_uint64_t x{};
    //     alignas(hardware_destructive_interference_size) std::atomic_uint64_t y{};
    // } twoCacheLiner;
    //
    // inline auto now() noexcept { return std::chrono::high_resolution_clock::now(); }

} // namespace cmdr::cross


#endif //CMDR_CXX11_CMDR_DEFS_HH
