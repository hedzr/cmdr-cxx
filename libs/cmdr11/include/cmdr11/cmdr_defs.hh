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

#ifndef unused
#define unused(x) \
    do { (void) (x); } while (0)
#endif

#ifndef UNUSED
#define UNUSED(x) \
    do { (void) (x); } while (0)
#endif

#ifndef __COPY
#define __COPY(m) this->m = o.m
#endif

#ifndef TEXT
#define TEXT(x) (#x)
#endif


//


// https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor/46177613
// https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive/8249232
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//define something for Windows (32-bit and 64-bit, this part is common)
#define OS_WINDOWS
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
constexpr bool always_false_v = always_false<T>::value;


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

#endif //CMDR_CXX11_CMDR_DEFS_HH
