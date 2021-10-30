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


namespace cmdr::test {

    inline std::ostream &build_time(std::ostream &os) {
        std::tm t{};
        std::istringstream tsi(__TIMESTAMP__);
        // tsi.imbue(std::locale("de_DE.utf-8"));
        tsi >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
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

    inline void test_for_macros() {
        std::cout << "CMDR_ENABLE_ASSERTIONS               : " << CMDR_ENABLE_ASSERTIONS << '\n';
        std::cout << "CMDR_ENABLE_PRECONDITION_CHECKS      : " << CMDR_ENABLE_PRECONDITION_CHECKS << '\n';
        std::cout << "CMDR_ENABLE_THREAD_POOL_READY_SIGNAL : " << CMDR_ENABLE_THREAD_POOL_READY_SIGNAL << '\n';
        std::cout << "CMDR_TEST_THREAD_POOL_DBGOUT         : " << CMDR_TEST_THREAD_POOL_DBGOUT << '\n';
        std::cout << "CMDR_UNIT_TEST                       : " << CMDR_UNIT_TEST << '\n';
#if defined(CMDR_ENABLE_VERBOSE_LOG)
        std::cout << "CMDR_ENABLE_VERBOSE_LOG              : " << CMDR_ENABLE_VERBOSE_LOG << '\n';
#endif

        std::cout << '\n'
                  << CMDR_PROJECT_NAME << " v" << CMDR_VERSION_STRING << '\n'
                  << CMDR_ARCHIVE_NAME << ": " << CMDR_DESCRIPTION << '\n'
                  << "         version: " << CMDR_VERSION_STR << '\n'
                  << "          branch: " << CMDR_GIT_BRANCH << '\n'
                  << "             tag: " << CMDR_GIT_TAG << " (" << CMDR_GIT_TAG_LONG << ")" << '\n'
                  << "            hash: " << CMDR_GIT_REV << " (" << CMDR_GIT_COMMIT_HASH << ")" << '\n'
                  << "             cpu: " << CMDR_CPU << '\n'
                  << "            arch: " << CMDR_CPU_ARCH << '\n'
                  << "       arch-name: " << CMDR_CPU_ARCH_NAME << '\n'
                  << "      build-name: " << CMDR_BUILD_NAME << '\n'
                  << "      build-time: " << build_time() << '\n'
                  << "       timestamp: " << chrono::format_time_point() << '\n'
                  << '\n';

        cmdr_debug("debug mode log enabled.");
        cmdr_trace("verbose log (trace mode) enabled.");
    }

} // namespace cmdr::test

#endif //CMDR_CXX11_Z_TEST_HH
