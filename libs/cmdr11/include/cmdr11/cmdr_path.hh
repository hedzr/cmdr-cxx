//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR11_CMDR_PATH_HH
#define CMDR11_CMDR_PATH_HH

#pragma once

#include <chrono>
#include <filesystem>
#include <string>

#include <cstdio>
#include <fstream>
#include <iostream>


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
#include <winioctl.h> // for FSCTL_SET_SPARSE
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

#include <sys/stat.h>

namespace cmdr::path {

#if defined(_WIN32)
    typedef struct ::_stat hz_stat;
#else
    typedef struct stat hz_stat;
#endif


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

#define MAX_PATH (PATH_MAX * 2 + 1)

    inline std::filesystem::path get_executable_path() {
        char rawPathName[MAX_PATH];
        char *p = realpath(PROC_SELF_EXE, rawPathName);
        // std::cout << "[DBG] exe path 1 : " << rawPathName << '\n';
        // std::cout << "[DBG] exe path 2 : " << p << '\n';
        return std::string(p ? p : rawPathName);
    }

    inline std::filesystem::path get_executable_dir() {
        auto executablePath = get_executable_path();
        // // char *executablePathStr = new char[executablePath.length() + 1];
        // char executablePathStr[MAX_PATH];
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

#define MAX_PATH PATH_MAX

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

    /**
     * @brief BUG in WIN32/WIN64
     * @param path 
     * @return 
     */
    inline const char *to_filename(std::filesystem::path const &path) {
#if defined(_WIN32)
        return path.u8string().c_str(); // BUG!!
#else
        return path.c_str();
#endif
    }

    inline std::string to_filename_h(std::filesystem::path const &path) {
#if defined(_WIN32)
        return path.u8string();
#else
        return path;
#endif
    }

} // namespace cmdr::path


namespace cmdr::io {

    inline std::ifstream open_file(std::filesystem::path const &name, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::binary) {
        std::ifstream ofs(name, mode);
        return ofs;
    }

    inline std::ofstream open_file_for_write(std::filesystem::path const &name, std::ios_base::openmode mode = std::ios_base::out | std::ios_base::binary) {
        std::ofstream ofs(name, mode);
        return ofs;
    }

    inline std::ofstream create_file(std::filesystem::path const &name, std::ios_base::openmode mode = std::ios_base::out | std::ios_base::binary) {
        std::ofstream ofs(name, mode);
        return ofs;
    }

    inline bool delete_file(std::filesystem::path const &name) {
        // unlink(name.c_str());
        return std::remove(name.u8string().c_str()) == 0;
    }
    inline bool delete_file(char const *filename) {
        return std::remove(filename) == 0;
    }

    inline void close_file(std::ofstream &&ofs) { ofs.close(); }
    inline void close_file(std::ofstream &ofs) { ofs.close(); }
    inline void close_file(std::ifstream &&ifs) { ifs.close(); }
    inline void close_file(std::ifstream &ifs) { ifs.close(); }
    inline void close_file(std::fstream &&fs) { fs.close(); }
    inline void close_file(std::fstream &fs) { fs.close(); }

    inline std::string read_file_content(std::ifstream &ifs) {
        ifs.ignore((std::numeric_limits<std::streamsize>::max)());
        std::string data(ifs.gcount(), 0);
        ifs.seekg(0);
        ifs.read(data.data(), data.size());
        return data;
    }

#ifdef _WIN32
// FSCTL_SET_SPARSE
#endif
    /**
 * @brief create a sparse file on disk, ready for linux, darwin, not test for windows.
 * @param name 
 * @param size 
 * @return 
 */
    inline bool create_sparse_file(std::filesystem::path const &name, std::size_t size) {
#ifdef _WIN32
        // https://stackoverflow.com/questions/4011508/how-to-create-a-sparse-file-on-ntfs

        // Use CreateFile as you would normally - Create file with whatever flags
        //and File Share attributes that works for you
        DWORD dwTemp;

        HANDLE hSparseFile = CreateFile(name.u8string().c_str(),
                                        GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL,
                                        CREATE_ALWAYS,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL);

        if (hSparseFile == INVALID_HANDLE_VALUE)
            return false;

        DeviceIoControl(hSparseFile,
                        FSCTL_SET_SPARSE,
                        NULL,
                        (DWORD) 0,
                        NULL,
                        0,
                        &dwTemp,
                        NULL);

        CloseHandle(hSparseFile);

        std::filesystem::resize_file(name, size);
#else
        std::ofstream ofs(name, std::ios_base::out | std::ios_base::binary);
        ofs.seekp(size - 1);
        char ch = '\0';
        ofs.write(&ch, 1);
#endif
        return true;
    }
} // namespace cmdr::io

namespace cmdr::path {

    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions
    // https://linux.die.net/man/2/stat
    inline hz_stat stat(std::filesystem::path const &name) {
        hz_stat st;
#ifdef _WIN32
        int result = _stat(name.u8string().c_str(), &st);
#else
        int result = ::stat(name.c_str(), &st);
#endif
        if (result != 0) {
            // check errno
            return st;
        }
        return st;
    }

    inline bool is_sparse_file(std::filesystem::path const &name) {
#ifdef _WIN32
        // https://www.codeproject.com/Articles/53000/Managing-Sparse-Files-on-Windows

        // Open the file for read
        HANDLE hFile = CreateFile(name.u8string().c_str(),
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return false;

        // Get file information
        BY_HANDLE_FILE_INFORMATION bhfi;
        GetFileInformationByHandle(hFile, &bhfi);
        CloseHandle(hFile);

        return (bhfi.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE);
#else
        struct stat st = stat(name);
        return (st.st_blksize * st.st_blocks < st.st_size);
#endif
    }

    /** create a directory if not exists */
    inline bool ensure_directory(std::filesystem::path const &name) {
        return fs::create_directories(name);
    }

    /** create a file if not exists */
    inline bool ensure_file(std::filesystem::path const &name) {
        if (!file_exists(name)) {
            io::close_file(io::create_file(name));
            return true;
        }
        return false;
    }

    /**
     * @brief get a const path representation of an empty pathname.
     * @return an implicit, static, global, empty, and const pathname.
     */
    inline std::filesystem::path const &empty_path() {
        static std::filesystem::path p;
        return p;
    }

    inline std::filesystem::path tmpname_for_stderr() {
        auto p = fs::temp_directory_path();
        p /= "logs";
        p /= "stderr.txt";
        ensure_directory(path::dirname(p));
        return p;
    }
    inline std::filesystem::path tmpname(std::filesystem::path const &name = empty_path()) {
        std::filesystem::path p = fs::temp_directory_path();
        p /= (name.empty() ? "1.bin" : name);
        ensure_directory(path::dirname(p));
        return p;
    }
    /**
     * @brief make a tmpname with name_template and an implicit number
     * @param name_template should be "some_text_%d_some_text". note the "%d" is required.
     * @return filename wrapped to std::filesystem::path
     */
    inline std::filesystem::path tmpname_autoincr(char const *name_template = nullptr) {
        std::filesystem::path p = fs::temp_directory_path();
        static int id = 0;
        char buf[MAX_PATH];
        sprintf(buf, name_template ? name_template : "_%05d", id++);
        p /= buf;
        ensure_directory(path::dirname(p));
        return p;
    }
    inline std::filesystem::path tmpdir() {
        return fs::temp_directory_path();
    }

} // namespace cmdr::path

//////// for executable_name() END


#endif // CMDR11_CMDR_PATH_HH
