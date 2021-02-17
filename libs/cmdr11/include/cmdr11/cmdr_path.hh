//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_PATH_HH
#define CMDR_CXX11_CMDR_PATH_HH

#pragma once

#include <filesystem>
#include <string>


namespace cmdr::path {

    std::string get_executable_path();

    std::string get_executable_dir();

    std::string merge(std::string pathA, std::string pathB);

    bool file_exists(const std::string &filePath);

} // namespace cmdr::path


#if defined(_WIN32)
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

    inline std::string get_executable_path() {
        char rawPathName[MAX_PATH];
        GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
        return std::string(rawPathName);
    }

    inline std::string get_executable_dir() {
        std::string executablePath = get_executable_path();
        char *exePath = new char[executablePath.length()];
        strcpy(exePath, executablePath.c_str());
        PathRemoveFileSpecA(exePath);
        std::string directory = std::string(exePath);
        delete[] exePath;
        return directory;
    }

    inline std::string merge(std::string pathA, std::string pathB) {
        char combined[MAX_PATH];
        PathCombineA(combined, pathA.c_str(), pathB.c_str());
        std::string mergedPath(combined);
        return mergedPath;
    }

#endif

#ifdef __linux__

    inline std::string get_executable_path() {
        char rawPathName[PATH_MAX * 2 + 1];
        char *p = realpath(PROC_SELF_EXE, rawPathName);
        // std::cout << "[DBG] exe path 1 : " << rawPathName << '\n';
        // std::cout << "[DBG] exe path 2 : " << p << '\n';
        return std::string(p ? p : rawPathName);
    }

    inline std::string get_executable_dir() {
        std::string executablePath = get_executable_path();
        // char *executablePathStr = new char[executablePath.length() + 1];
        char executablePathStr[PATH_MAX * 2 + 1];
        strcpy(executablePathStr, executablePath.c_str());
        char *executableDir = dirname(executablePathStr);
        std::string ret(executableDir);
        // std::cout << "[DBG] exe path : " << executablePathStr << '\n';
        // std::cout << "[DBG] exe dir  : " << executableDir << '\n';
        return ret;
    }

    inline std::string merge(std::string pathA, std::string pathB) {
        return pathA + "/" + pathB;
    }

#endif

#ifdef __APPLE__

    inline std::string get_executable_path() {
        char rawPathName[PATH_MAX];
        char realPathName[PATH_MAX];
        auto rawPathSize = (uint32_t) sizeof(rawPathName);

        if (!_NSGetExecutablePath(rawPathName, &rawPathSize)) {
            realpath(rawPathName, realPathName);
        }
        return std::string(realPathName);
    }

    inline std::string get_executable_dir() {
        std::string executablePath = get_executable_path();
        char *executablePathStr = new char[executablePath.length() + 1];
        strcpy(executablePathStr, executablePath.c_str());
        char *executableDir = dirname(executablePathStr);
        delete[] executablePathStr;
        return std::string(executableDir);
    }

    inline std::string merge(std::string const &pathA, std::string const &pathB) {
        return pathA + "/" + pathB;
    }

    inline std::filesystem::path get_current_dir() {
        return std::filesystem::current_path();
    }

#endif


    inline bool file_exists(const std::string &filePath) {
        return access(filePath.c_str(), 0) == 0;
    }

} // namespace cmdr::path


//////// for executable_name()

// #pragma once

//
// https://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros
//
#ifdef __cpp_lib_filesystem

#include <filesystem>


#else
#include <experimental/filesystem>

namespace std {
    namespace filesystem = experimental::filesystem;
}
#endif

#ifdef _WIN32
#include <windows.h> //GetModuleFileNameW
#else

#include <climits>
#include <unistd.h> // for readlink


#endif

namespace cmdr::path {
    std::filesystem::path executable_name();

    inline std::filesystem::path executable_name() {
        auto path = std::filesystem::path(get_executable_path());
        return path.filename();
    }
} // namespace cmdr::path

//////// for executable_name() END


#endif //CMDR_CXX11_CMDR_PATH_HH
