//
// Created by Hedzr Yeh on 2021/8/6.
//

#ifndef CMDR_CXX11_Z_TEST_HH
#define CMDR_CXX11_Z_TEST_HH

#include <algorithm>
#include <ctime>
#include <functional>
#include <memory>

#include "cmdr_chrono.hh"
#include "cmdr_dbg.hh"
#include "cmdr_log.hh"

#include "cmdr-config-base.hh"


namespace cmdr::test {

  inline std::ostream &build_time(std::ostream &os) {
    std::tm t{};
#if defined(__BUILD_TIMESTAMP)
    std::istringstream tsi(__BUILD_TIMESTAMP);
    tsi >> std::get_time(&t, "%Y-%m-%dT%H:%M:%SZ");
#elif defined(BUILD_TIMESTAMP)
    std::istringstream tsi(BUILD_TIMESTAMP);
    tsi >> std::get_time(&t, "%b %d %Y %H:%M:%S");
#elif defined(BUILD_TIME)
    std::istringstream tsi(BUILD_TIME);
    tsi >> std::get_time(&t, "%b %d %Y %H:%M:%S");
#elif defined(CONFIGURE_TIMESTAMP)
    std::istringstream tsi(CONFIGURE_TIMESTAMP);
    tsi >> std::get_time(&t, "%Y-%m-%dT%H:%M:%SZ");
#elif defined(__CMAKE_CONFIGURE_TIMESTAMP)
    std::istringstream tsi(__CMAKE_CONFIGURE_TIMESTAMP);
    tsi >> std::get_time(&t, "%Y-%m-%dT%H:%M:%SZ");
#elif defined(__DATE__)
    std::istringstream tsi(__DATE__ " " __TIME__);
    tsi >> std::get_time(&t, "%b %d %Y %H:%M:%S");
#else
    std::istringstream tsi(__TIMESTAMP__);
    tsi >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
#endif
    // tsi.imbue(std::locale("de_DE.utf-8"));
    // tsi >> std::get_time(&t, "%Y-%m-%dT%H:%M:%SZ");
    // tsi >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
    // std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
    // std::ostringstream ts;
    // ts << std::put_time(&t, "%Y-%m-%dT%H:%M:%S");
    return os << std::put_time(&t, "%FT%T%z");
  }
  inline std::string build_time() {
    std::ostringstream ts;
    build_time(ts);
    return ts.str();
  }

  inline std::ostream &timestamp_format(std::ostream &os, char const *timestamp = __TIMESTAMP__) {
    std::tm t{};
    std::istringstream tsi(timestamp);
    // tsi.imbue(std::locale("de_DE.utf-8"));
    tsi >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
    return os << std::put_time(&t, "%FT%T%z");
  }
  inline std::string timestamp_format(char const *timestamp = __TIMESTAMP__) {
    std::ostringstream ts;
    timestamp_format(ts, timestamp);
    return ts.str();
  }

  inline void test_for_macros() {
#if defined(CMDR_ENABLE_ASSERTIONS)
    std::cout << "CMDR_ENABLE_ASSERTIONS               : " << CMDR_ENABLE_ASSERTIONS << '\n';
#endif
#if defined(CMDR_ENABLE_PRECONDITION_CHECKS)
    std::cout << "CMDR_ENABLE_PRECONDITION_CHECKS      : " << CMDR_ENABLE_PRECONDITION_CHECKS << '\n';
#endif
#if defined(CMDR_ENABLE_THREAD_POOL_READY_SIGNAL)
    std::cout << "CMDR_ENABLE_THREAD_POOL_READY_SIGNAL : " << CMDR_ENABLE_THREAD_POOL_READY_SIGNAL << '\n';
#endif
#if defined(CMDR_TEST_THREAD_POOL_DBGOUT)
    std::cout << "CMDR_TEST_THREAD_POOL_DBGOUT         : " << CMDR_TEST_THREAD_POOL_DBGOUT << '\n';
#endif
#if defined(CMDR_UNIT_TEST)
    std::cout << "CMDR_UNIT_TEST                       : " << CMDR_UNIT_TEST << '\n';
#endif
#if defined(CMDR_ENABLE_VERBOSE_LOG)
    std::cout << "CMDR_ENABLE_VERBOSE_LOG              : " << CMDR_ENABLE_VERBOSE_LOG << '\n';
#endif

    std::cout << '\n'
              // << CMDR_PROJECT_NAME << " v" << CMDR_VERSION_STRING << '\n'
              << CMDR_ARCHIVE_NAME << ": " << CMDR_DESCRIPTION << '\n'
              << '\n'
              << "              version: " << CMDR_VERSION_STR << '\n'
              << "               branch: " << CMDR_GIT_BRANCH << '\n'
              << "                  tag: " << CMDR_GIT_TAG << " (" << CMDR_GIT_TAG_LONG << ")" << '\n'
              << "                 hash: " << CMDR_GIT_REV << " (" << CMDR_GIT_COMMIT_HASH << ")" << '\n'
              << "                  cpu: " << CMDR_CPU << '\n'
              << "                 arch: " << CMDR_CPU_ARCH << '\n'
              << "            arch-name: " << CMDR_CPU_ARCH_NAME << '\n'
              << "           build-name: " << CMDR_BUILD_NAME << '\n'
              << "    build-time (cmdr): " << build_time() << '\n'
              << " last-modified (cmdr): " << timestamp_format() << '\n'
              << "            timestamp: " << chrono::format_time_point() << '\n'
              << '\n'
              << "     compiled by: " << cmdr::cross::compiler_name() << '\n'
              << "     __cplusplus: 0x" << std::hex << std::setfill('0') << std::setw(8) << __cplusplus << ' ' << '(' << std::dec << __cplusplus << ')' << '\n'
              << '\n';

    cmdr_debug("debug mode log enabled.");
    cmdr_trace("verbose log (trace mode) enabled.");
  }

} // namespace cmdr::test

#endif // CMDR_CXX11_Z_TEST_HH
