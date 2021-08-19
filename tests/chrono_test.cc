//
// Created by Hedzr Yeh on 2021/1/21.
//

#include <thread>

#include "cmdr11/cmdr_chrono.hh"
#include "cmdr11/cmdr_process.hh"
#include "cmdr11/cmdr_x_test.hh"

#include "cmdr11/cmdr_log.hh"

void foo1() {
    cmdr_print("foo1 hit.");
}

void test_c_style(struct timeval &tv) {
#ifdef _WIN32
    // char fmt[64];
    // char buf[256];
    // time_t rawtime;
    // struct tm * timeinfo;
    // time (&rawtime);
    // timeinfo = localtime (&rawtime);
    // strftime(fmt, sizeof(fmt), "%H:%M:%S.%%06u", tm);
    // strftime (buf, sizeof(buf), "Timestamp = %A, %d/%m/%Y %T\0", timeinfo);
    // printf("%s\n", buf);
    // snprintf(buf, sizeof(buf), fmt, tv.tv_usec);
    // printf("%s\n", buf);
    UNUSED(tv);
#else
    char fmt[64];
    char buf[64];
    struct tm *tm;
    tm = localtime(&tv.tv_sec);
    strftime(fmt, sizeof(fmt), "%H:%M:%S.%%06u", tm);
    snprintf(buf, sizeof(buf), fmt, tv.tv_usec);
    printf("%s\n", buf);
#endif
}

void test_cpp_style(std::chrono::system_clock::time_point &now, std::tm &now_tm) {
    //cmdr::process::exec date_cmd("date  +\"%T,%N\"");
    {
        using namespace std::chrono;
        // auto now = system_clock::now();
        auto now_ms = time_point_cast<milliseconds>(now);

        auto value = now_ms.time_since_epoch();
        long duration = (long) value.count();

        milliseconds dur(duration);

        time_point<system_clock> dt(dur);

        if (dt != now_ms)
            std::cout << "Failure." << '\n';
        else
            std::cout << "Success." << '\n';
    }

    {
        using namespace std::chrono;
        // Get current time with precision of milliseconds
        auto now11 = time_point_cast<milliseconds>(now); // system_clock::now());
        // sys_milliseconds is type time_point<system_clock, milliseconds>
        using sys_milliseconds = decltype(now11);
        // Convert time_point to signed integral type
        auto integral_duration = now11.time_since_epoch().count();
        // Convert signed integral type to time_point
        sys_milliseconds dt{milliseconds{integral_duration}};
        // test
        if (dt != now11)
            std::cout << "Failure." << '\n';
        else
            std::cout << "Success." << '\n';
    }

    {
        // std::chrono::time_point< std::chrono::system_clock > now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();

        /* UTC: -3:00 = 24 - 3 = 21 */
        typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<21>>::type> Days;

        Days days = std::chrono::duration_cast<Days>(duration);
        duration -= days;

        auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
        duration -= hours;

        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
        duration -= minutes;

        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        duration -= seconds;

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        duration -= milliseconds;

        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
        duration -= microseconds;

        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

        // C library function - localtime()
        // https://www.tutorialspoint.com/c_standard_library/c_function_localtime.htm
        //
        // struct tm {
        //    int tm_sec;         // seconds,  range 0 to 59
        //    int tm_min;         // minutes, range 0 to 59
        //    int tm_hour;        // hours, range 0 to 23
        //    int tm_mday;        // day of the month, range 1 to 31
        //    int tm_mon;         // month, range 0 to 11
        //    int tm_year;        // The number of years since 1900
        //    int tm_wday;        // day of the week, range 0 to 6
        //    int tm_yday;        // day in the year, range 0 to 365
        //    int tm_isdst;       // daylight saving time
        // };

        time_t theTime = time(NULL);
        struct tm *aTime = localtime(&theTime);

        std::cout << days.count() << " days since epoch or "
                  << days.count() / 365.2524 << " years since epoch. The time is now "
                  << aTime->tm_hour << ":"
                  << minutes.count() << ":"
                  << seconds.count() << ","
                  << milliseconds.count() << ":"
                  << microseconds.count() << ":"
                  << nanoseconds.count() << '\n';
    }

    // std::cout << "date command: " << date_cmd << '\n';

    using iom = cmdr::chrono::iom;
    std::cout << iom::fmtflags::gmt << iom::fmtflags::ns << "time_point (ns): os << " << now << '\n';
    std::cout << iom::fmtflags::gmt << iom::fmtflags::us << "time_point (us): os << " << now << '\n';
    std::cout << iom::fmtflags::gmt << iom::fmtflags::ms << "time_point (ms): os << " << now << '\n';
    std::cout << "std::tm:    os << " << now_tm << '\n';

    // {
    //     std::chrono::high_resolution_clock::time_point nowh = std::chrono::high_resolution_clock::now();
    //     std::cout << iom::gmt << iom::ns << "time_point: hires " << nowh << '\n';
    // }

    std::chrono::system_clock::time_point now2 = std::chrono::system_clock::now();
    auto d = now2 - now;
    std::cout << "duration:   os << " << d << '\n';
}

void test_cpp_style(std::chrono::system_clock::time_point &now) {
    std::time_t now_tt = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_tt);
    test_cpp_style(now, now_tm);
}

void test_time_now() {
    struct timeval tv = cmdr::chrono::get_system_clock_in_us();
    std::chrono::system_clock::time_point now = cmdr::chrono::now();
    auto now1 = cmdr::chrono::clock::now();

    test_c_style(tv);
    test_cpp_style(now);

    using iom = cmdr::chrono::iom;
    std::cout << iom::fmtflags::local << iom::fmtflags::ns << "time_point (clock): " << now1 << '\n';
    std::cout << iom::fmtflags::local << iom::fmtflags::us << "time_point (clock): " << now1 << '\n';
    std::cout << 1 << '\n'
              << '\n';
}

template<class _Rep, class _Period>
void echo(std::chrono::duration<_Rep, _Period> d) {
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(d);
    std::cout << "duration " << d.count() << " (" << ns.count() << ") = " << d << '\n';
}

void test_format_duration() {
    using namespace std::literals::chrono_literals;
    for (auto d : std::vector<std::chrono::duration<long double, std::ratio<60>>>{
                 3ns,
                 800ms,
                 59.739us,
                 0.75min,
                 501ns,
                 730us,
                 233ms,
                 7s,
                 7.2s,
                 1024h,
                 89.843204843s,
         }) {
        echo(d);
    }
}

void test_high_resolution_duration() {
    auto then = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now - then;

    auto clean_duration = cmdr::chrono::detail::break_down_durations<std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds>(duration);
    //    auto timeInMicroSec = std::chrono::duration_cast<std::chrono::microseconds>(duration); // base in Microsec.
    std::cout << std::get<0>(clean_duration).count() << "::" << std::get<1>(clean_duration).count() << "::" << std::get<2>(clean_duration).count() << "\n";
}

void test_try_parse_by() {
    using Clock = std::chrono::system_clock;

    for (auto &time_str : {
                 "11:01:37",
                 "1937-1-29 3:59:59",
         }) {
        std::tm tm = cmdr::chrono::time_point_2_tm(Clock::now());
        typename Clock::time_point tp;
        if (cmdr::chrono::try_parse_by(tm, time_str, "%H:%M:%S", "%Y-%m-%d %H:%M:%S", "%Y/%m/%d %H:%M:%S")) {
            tp = cmdr::chrono::tm_2_time_point(&tm);
            cmdr_print("  - time '%s' parsed: tp = %s",
                       time_str,
                       // _twl.size(), hit, loop,
                       // cmdr::chrono::format_duration(d).c_str(),
                       // cmdr::chrono::format_time_point(tp).c_str(),
                       cmdr::chrono::format_time_point_to_local(tp).c_str());
        }
    }
}

void test_last_day_at_this_month() {
    namespace chr = cmdr::chrono;
    using clock = std::chrono::system_clock;
    using time_point = clock::time_point;
    using namespace std::literals::chrono_literals;

    struct testcase {
        const char *desc;
        int offset;
        time_point now, expected;
    };
#define NOW_CASE(now_str, expected_str, desc, ofs) \
    testcase { desc, ofs, chr::parse_datetime(now_str), chr::parse_datetime(expected_str) }

    for (auto t : {
                 // Month .. Year
                 NOW_CASE("2021-08-05", "2021-08-29", "day -3", 3),
                 NOW_CASE("2021-08-05", "2021-08-22", "day -10", 10),
                 NOW_CASE("2021-08-05", "2021-08-17", "day -15", 15),
                 NOW_CASE("2021-08-05", "2021-08-07", "day -25", 25),

         }) {
        auto now = t.now;
        std::tm tm = chr::time_point_2_tm(now);
        tm = chr::last_day_at_this_month(tm, t.offset, 1);
        auto pt = chr::tm_2_time_point(tm);
        cmdr_print("%40s: %s -> %s", t.desc, chr::format_time_point_to_local(now).c_str(), chr::format_time_point_to_local(pt).c_str());

        auto tmp = t.expected;
        if (!chr::duration_is_zero(tmp)) {
            if (chr::compare_date_part(pt, tmp) != 0) {
                cmdr_print("%40s: ERROR: expecting %s but got %s", " ", chr::format_time_point_to_local(tmp).c_str(), chr::format_time_point_to_local(pt).c_str());
                exit(-1);
            }
        }
    }

#undef NOW_CASE
}

void test_last_day_at_this_year() {
    namespace chr = cmdr::chrono;
    using clock = std::chrono::system_clock;
    using time_point = clock::time_point;
    using namespace std::literals::chrono_literals;

    struct testcase {
        const char *desc;
        int offset;
        time_point now, expected;
    };
#define NOW_CASE(now_str, expected_str, desc, ofs) \
    testcase { desc, ofs, chr::parse_datetime(now_str), chr::parse_datetime(expected_str) }

    for (auto t : {
                 // Month .. Year
                 NOW_CASE("2021-08-05", "2021-12-29", "day -3", 3),
                 NOW_CASE("2021-08-05", "2021-12-22", "day -10", 10),
                 NOW_CASE("2021-08-05", "2021-12-17", "day -15", 15),
                 NOW_CASE("2021-08-05", "2021-12-07", "day -25", 25),
                 NOW_CASE("2021-08-05", "2021-1-1", "day -365", 365),

         }) {
        auto now = t.now;
        std::tm tm = chr::time_point_2_tm(now);
        tm = chr::last_day_at_this_year(tm, t.offset);
        auto pt = chr::tm_2_time_point(tm);
        cmdr_print("%40s: %s -> %s", t.desc, chr::format_time_point_to_local(now).c_str(), chr::format_time_point_to_local(pt).c_str());

        auto tmp = t.expected;
        if (!chr::duration_is_zero(tmp)) {
            if (chr::compare_date_part(pt, tmp) != 0) {
                cmdr_print("%40s: ERROR: expecting %s but got %s", " ", chr::format_time_point_to_local(tmp).c_str(), chr::format_time_point_to_local(pt).c_str());
                exit(-1);
            }
        }
    }

#undef NOW_CASE
}

int main() {
    cmdr::chrono::high_res_duration _hrd;

    CMDR_TEST_FOR(test_try_parse_by);
    CMDR_TEST_FOR(test_time_now);
    CMDR_TEST_FOR(test_format_duration);

    CMDR_TEST_FOR(test_last_day_at_this_year);
    CMDR_TEST_FOR(test_last_day_at_this_month);

#ifndef _WIN32
    {
        struct timespec ts;
        clock_getres(CLOCK_REALTIME, &ts);
        std::cout << ts.tv_sec << ',' << ts.tv_nsec << '\n';
        clock_getres(CLOCK_MONOTONIC, &ts);
        std::cout << ts.tv_sec << ',' << ts.tv_nsec << '\n';
    }
#endif

    test_high_resolution_duration();
}