//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_DEFS_HH
#define CMDR_CXX11_CMDR_DEFS_HH


#include <initializer_list>
#include <iostream>
#include <utility>
#include <vector>


#if !defined(_DEBUG) && defined(DEBUG)
#define _DEBUG DEBUG
#endif

#ifdef __clang__

//#ifndef UNUSED
//#define UNUSED(...) [__VA_ARGS__](){}
//#endif
template<typename... Args>
void UNUSED([[maybe_unused]] Args &&...args) {
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


#if defined(ENABLE_ASSERTS)
#define CMDR_ASSERT(...) assert(__VA_ARGS__)
#else
#define CMDR_ASSERT(...) UNUSED(__VA_ARGS__)
#endif


#ifndef __COPY
#define __COPY(m) this->m = o.m
#endif

#if !defined(_WIN32)
#ifndef TEXT
#define TEXT(x) (#x)
#endif
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
[[maybe_unused]] size_t countof(T (&arr)[N]) {
    UNUSED(arr);
    return std::extent<T[N]>::value;
}


//


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


//


template<typename T>
struct always_false : std::false_type {};

template<typename T>
[[maybe_unused]] constexpr bool always_false_v = always_false<T>::value;


typedef const char *const_chars;
typedef std::vector<std::string> string_array;


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

//template<class T=std::string>
//constexpr T UNSORTED_GROUP = T("1230.Unsorted");
//
//template<class T=std::string>
//constexpr T SYS_MGMT_GROUP = T("9000.System Management");

// #if defined(CMDR_ENABLE_VERBOSE_LOG)
// #include <spdlog/spdlog.h>
// #endif


////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#if defined(OS_WIN)
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
        _putenv(os.str().c_str());
    }

    inline time_t time(time_t *_t = nullptr) {
        return ::time(_t);
    }
    inline struct tm *gmtime(time_t const *_t = nullptr, struct tm *_tm = nullptr) {
        gmtime_s(_tm, _t);
        return _tm;
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
    inline struct tm *gmtime(time_t const *_t = nullptr, struct tm *_tm_never_used = nullptr) {
        UNUSED(_tm_never_used);
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


#endif //CMDR_CXX11_CMDR_DEFS_HH
