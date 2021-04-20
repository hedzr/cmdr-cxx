//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR11_CMDR_PATH_HH
#define CMDR11_CMDR_PATH_HH

#pragma once

#include <chrono>
#include <filesystem>
#include <string>


namespace cmdr::path {
    
    namespace fs = std::filesystem;

    std::filesystem::path get_executable_path();

    std::filesystem::path get_executable_dir();

    std::filesystem::path merge(std::string pathA, std::string pathB);

    bool file_exists(const std::string &filePath);
    bool file_exists(std::filesystem::path const &&filePath);

} // namespace cmdr::path


#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
// #define _CRT_SECURE_NO_WARNINGS
#include <Shlwapi.h>
#include <io.h>
#include <windows.h>

#define access _access_s
#endif

#ifdef __APPLE__

#include <climits>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <unistd.h>

#endif

#ifdef __linux__
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#if defined(__sun)
#define PROC_SELF_EXE "/proc/self/path/a.out"
#else
#define PROC_SELF_EXE "/proc/self/exe"
#endif

#endif

namespace cmdr::path {

#if defined(_WIN32)

    inline std::filesystem::path get_executable_path() {
        char rawPathName[MAX_PATH];
        GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
        return std::string(rawPathName);
    }

    inline std::filesystem::path get_executable_dir() {
        auto executablePath = get_executable_path();
        // char *exePath = new char[executablePath.length()];
        // strcpy(exePath, executablePath.c_str());
        // PathRemoveFileSpecA(exePath);
        // std::string directory = std::string(exePath);
        // delete[] exePath;
        return executablePath.parent_path();
    }

    inline std::filesystem::path merge(std::string pathA, std::string pathB) {
        char combined[MAX_PATH];
        PathCombineA(combined, pathA.c_str(), pathB.c_str());
        std::string mergedPath(combined);
        return mergedPath;
    }

#endif

#ifdef __linux__

    inline std::filesystem::path get_executable_path() {
        char rawPathName[PATH_MAX * 2 + 1];
        char *p = realpath(PROC_SELF_EXE, rawPathName);
        // std::cout << "[DBG] exe path 1 : " << rawPathName << '\n';
        // std::cout << "[DBG] exe path 2 : " << p << '\n';
        return std::string(p ? p : rawPathName);
    }

    inline std::filesystem::path get_executable_dir() {
        auto executablePath = get_executable_path();
        // // char *executablePathStr = new char[executablePath.length() + 1];
        // char executablePathStr[PATH_MAX * 2 + 1];
        // strcpy(executablePathStr, executablePath.c_str());
        // char *executableDir = dirname(executablePathStr);
        // std::string ret(executableDir);
        // std::cout << "[DBG] exe path : " << executablePathStr << '\n';
        // std::cout << "[DBG] exe dir  : " << executableDir << '\n';
        return executablePath.parent_path();
    }

    inline std::filesystem::path merge(std::string pathA, std::string pathB) {
        return pathA + "/" + pathB;
    }

#endif

#ifdef __APPLE__

    inline std::filesystem::path get_executable_path() {
        char rawPathName[PATH_MAX];
        char realPathName[PATH_MAX];
        auto rawPathSize = (uint32_t) sizeof(rawPathName);

        if (!_NSGetExecutablePath(rawPathName, &rawPathSize)) {
            realpath(rawPathName, realPathName);
        }
        return std::filesystem::path(realPathName);
    }

    inline std::filesystem::path get_executable_dir() {
        auto executablePath = get_executable_path();
        return executablePath.parent_path();
        //char *executablePathStr = new char[executablePath.length() + 1];
        //strcpy(executablePathStr, executablePath.c_str());
        //char *executableDir = dirname(executablePathStr);
        //delete[] executablePathStr;
        //return std::filesystem::path(executableDir);
    }

    // Join the two path, ...
    inline std::filesystem::path merge(std::string const &pathA, std::string const &pathB) {
        auto p = fs::path(pathA);
        p /= pathB;
        return p;
    }

#endif


    // Join the paths...
    template<class T, class... Args>
    fs::path &join(fs::path &p, T const &arg, Args &...args) {
        p /= arg;
        if constexpr (sizeof...(Args) > 0) {
            join(p, args...);
        }
        return p;
    }
    // Join the paths...
    template<class A, class... Args,
            std::enable_if_t<!std::is_same<A, std::filesystem::path>::value, int> = 0>
    fs::path join(A const &arg, Args... args) {
        auto p = fs::path(arg);
        if constexpr (sizeof...(Args) > 0) {
            join(p, args...);
        }
        return p;
    }

    // for '/tmp/a/b/c.txt', returns '/tmp/a/b'
    inline std::filesystem::path dirname(std::filesystem::path &p) { return p.parent_path(); }
    // for '/tmp/a/b/c.txt', returns 'c.txt'
    inline std::filesystem::path basename(std::filesystem::path &p) { return p.filename(); }
    // for '/tmp/a/b/c.txt', returns '.txt'
    // for "foo.bar.baz.tar", the loops on extname(p) will returns: .tar, .baz, .bar.
    inline std::filesystem::path extname(std::filesystem::path &p) { return p.extension(); }
    // for '/tmp/a/b/c.txt', returns 'c'
    // for '/tmp/a/b/.txt', returns '.txt'
    inline std::filesystem::path basename_without_ext(std::filesystem::path &p) { return p.stem(); }


    inline std::filesystem::path get_current_dir() {
        return std::filesystem::current_path();
    }

    inline std::filesystem::path get_current_directory() { return get_current_dir(); }
    inline std::filesystem::path get_pwd() { return get_current_dir(); }
    inline std::filesystem::path get_cwd() { return get_current_dir(); }
    

    inline bool file_exists(const std::string &filePath) {
        return access(filePath.c_str(), 0) == 0;
    }

    inline bool file_exists(std::filesystem::path const &filePath) {
        return access(filePath.u8string().c_str(), 0) == 0;
    }

} // namespace cmdr::path


//////// for executable_name()

// #pragma once

//
// https://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros
//
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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
// #define _CRT_SECURE_NO_WARNINGS
#include <windows.h> //GetModuleFileNameW
#else

#include <climits>
#include <unistd.h> // for readlink

#endif

namespace cmdr::path {
    
    inline std::filesystem::path executable_name_as_path() {
        auto path = std::filesystem::path(get_executable_path());
        return path.filename();
    }
    inline std::filesystem::path executable_name() {
        return executable_name_as_path();
        //auto path = std::filesystem::path(get_executable_path());
        //return path.filename().c_str();
    }

    inline bool ensure_directory(std::filesystem::path name) {
        return fs::create_directories(name);
    }

    inline std::filesystem::path tmpname() {
        auto name1 = fs::temp_directory_path();
        auto p = path::join(name1, "logs", "stderr.txt");
        ensure_directory(path::dirname(p));
        return p;
    }
    inline std::filesystem::path tmpdir() {
        return fs::temp_directory_path();
    }

} // namespace cmdr::path

//////// for executable_name() END


#endif // CMDR11_CMDR_PATH_HH
