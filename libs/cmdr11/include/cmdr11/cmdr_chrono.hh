//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef CMDR_CXX11_CMDR_CHRONO_HH
#define CMDR_CXX11_CMDR_CHRONO_HH


#include <chrono>
#include <functional>
#include <string>
#include <tuple>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#if defined(_WIN32)
#include <chrono>
#include <winsock.h>

inline int gettimeofday(struct timeval *tp, struct timezone * /* tzp */) {
  namespace sc                 = std::chrono;
  sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
  sc::seconds s                = sc::duration_cast<sc::seconds>(d);
  tp->tv_sec                   = (long) s.count();
  tp->tv_usec                  = (long) sc::duration_cast<sc::microseconds>(d - s).count();
  return 0;
}

namespace cmdr::chrono::detail {
  const __int64 exp7 = 10000000i64;           // 1E+7     //C-file part
  const __int64 exp9 = 1000000000i64;         // 1E+9
  const __int64 w2ux = 116444736000000000i64; // 1.jan1601 to 1.jan1970
} // namespace cmdr::chrono::detail

inline void unix_time(struct timespec *spec) {
  __int64 wintime;
  ::GetSystemTimeAsFileTime((FILETIME *) &wintime);
  wintime -= cmdr::chrono::detail::w2ux;
  spec->tv_sec  = wintime / cmdr::chrono::detail::exp7;
  spec->tv_nsec = wintime % cmdr::chrono::detail::exp7 * 100;
}
inline int clock_gettime(int, timespec *spec) {
  static struct timespec startspec;
  static double ticks2nano;
  static __int64 startticks, tps = 0;
  __int64 tmp, curticks;
  ::QueryPerformanceFrequency((LARGE_INTEGER *) &tmp); // some strange system can
  if (tps != tmp) {
    tps = tmp; // init ~~ONCE         //possibly change freq ?
    ::QueryPerformanceCounter((LARGE_INTEGER *) &startticks);
    unix_time(&startspec);
    ticks2nano = (double) cmdr::chrono::detail::exp9 / tps;
  }
  ::QueryPerformanceCounter((LARGE_INTEGER *) &curticks);
  curticks -= startticks;
  spec->tv_sec  = startspec.tv_sec + (curticks / tps);
  spec->tv_nsec = (long) (startspec.tv_nsec + (double) (curticks % tps) * ticks2nano);
  if (!(spec->tv_nsec < cmdr::chrono::detail::exp9)) {
    spec->tv_sec++;
    spec->tv_nsec -= cmdr::chrono::detail::exp9;
  }
  return 0;
}
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#else
#include <sys/time.h> // gettimeofday
#endif                // _WIN32

#include "cmdr_defs.hh"

// type checks
namespace cmdr::chrono {


  template<typename T, typename _ = void>
  struct is_duration : std::false_type {};

  template<typename... Ts>
  struct is_duration_helper {};

  template<typename T>
  struct is_duration<
      T,
      std::conditional_t<
          false,
          is_duration_helper<
              typename T::rep,
              typename T::period,
              decltype(std::declval<T>().count()),
              decltype(std::declval<T>().zero())
#ifndef _WIN32
                  ,
              decltype(std::declval<T>().min()),
              decltype(std::declval<T>().max())
#endif
              >,
          void>> : public std::true_type {
  };

  template<typename T>
  constexpr bool is_duration_v = is_duration<T>::value;

} // namespace cmdr::chrono

// get_system_clock_in_us
namespace cmdr::chrono {

  inline struct timeval get_system_clock_in_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv;
  }

  inline struct timespec get_system_clock_in_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts;
  }

  inline long get_system_clock_ns_part() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_nsec;
  }

  inline std::chrono::system_clock::time_point now() {
    return std::chrono::system_clock::now();
  }

  /**
   * @brief simple wall clock (with nanosecond accuracy) class.
   *
   * In some systems, the wall clock has only millisecond or microsecond accuracy (such as darwin), not nanosecond (most of linux releases).
   * So `clock` class is not completely ns accuracy.
   */
  class clock {
  public:
    static clock now() { return clock(); }
    clock()
        : _now(std::chrono::system_clock::now()) {
      // struct timespec ts;
      // clock_gettime(CLOCK_REALTIME, &ts);
      // _nsec = ts.tv_nsec;
    }
    ~clock() {}
    std::size_t in_nsec() const;
    std::size_t nsec() const;
    std::ostream &serialize(std::ostream &os, const char *format = "%Y-%m-%d %H:%M:%S") const;

  private:
    std::chrono::system_clock::time_point _now;
    // long _nsec;
  };

} // namespace cmdr::chrono

// high_res_duration
namespace cmdr::chrono {

  namespace detail {
    template<class... Durations, class DurationIn>
    inline std::tuple<Durations...> break_down_durations(DurationIn d) {
      std::tuple<Durations...> retval;
      using discard = int[];
      (void) discard{0, (void((
                             (std::get<Durations>(retval) = std::chrono::duration_cast<Durations>(d)),
                             (d -= std::chrono::duration_cast<DurationIn>(std::get<Durations>(retval))))),
                         0)...};
      return retval;
    }
  } // namespace detail

  /**
   * @brief a high resolution time span calculator
   *
   * @details Usage:
   *
   *   Just make it as a stack variable, for example:
   * @code{c++}
   *    void yours(){
   *          cmdr::chrono::high_res_duration hrd;
   *
   *          //...
   *
   *          // at the exiting this function, hrd will print a timing log line.
   *    }
   * @endcode
   *
   * If you post a callback at constructor, the default printer could be
   * overwritten by a false return in your callback function. Here is a
   * sample:
   * @code{c++}
   *     cmdr::chrono::high_res_duration hrd([](auto duration) -> bool {
   *       std::cout << "It took " << duration << '\n';
   *       return false;
   *     });
   * @endcode
   *
   */
  class high_res_duration {
  public:
    high_res_duration(std::function<bool(std::chrono::high_resolution_clock::duration duration)> const &fn = nullptr)
        : _then(std::chrono::high_resolution_clock::now())
        , _cb(fn) {}
    ~high_res_duration() {
      _now          = std::chrono::high_resolution_clock::now();
      auto duration = _now - _then;

      // auto [ss, ms, us] = break_down_durations<std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds>(duration);

      bool ok{true};
      if (_cb)
        ok = _cb(duration);
      if (ok) {
        // auto clean_duration = break_down_durations<std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds>(duration);
        //    auto timeInMicroSec = std::chrono::duration_cast<std::chrono::microseconds>(duration); // base in Microsec.
        // std::cout << "It took " << std::get<0>(clean_duration).count() << "::" << std::get<1>(clean_duration).count() << "::" << std::get<2>(clean_duration).count() << "\n";
        auto const d = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        print_duration(std::cout, d);
      }
    }

    template<typename T,
             std::enable_if_t<cmdr::chrono::is_duration<T>::value, bool> = true>
    void print_duration(std::ostream &os, T const &v);

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> _then, _now;
    std::function<bool(std::chrono::high_resolution_clock::duration)> _cb;
  };


} // namespace cmdr::chrono

// iom
namespace cmdr::chrono {

  /**
   * @brief like std::ios, iom provides a set of flags for tuning the output as stream formatting.
   *
   * @detail For Example:
   * @code{c++}
   * using iom = cmdr::chrono::iom;
   * std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
   * std::cout << iom::local << iom::ns << "time_point: os << " << now << iom::clear << '\n';
   * @endcode
   *
   * @note iom is not thread-safe.
   */
  class iom {
  public:
    enum class fmtflags {
      nothing           = 0x0001,
      ms                = 0x0001,
      us                = 0x0002,
      ns                = 0x0003,
      mask_extra_fields = 0x000f,
      gmt               = 0x1000,
      local             = 0x2000,
      gmt_or_local      = 0x4000,
      clear             = 0x0000,
    };
    // typedef u_int32_t fmtflags;
    // static const fmtflags nothing = 0x0001;
    // static const fmtflags ms = 0x0001;
    // static const fmtflags us = 0x0002;
    // static const fmtflags ns = 0x0003;
    // static const fmtflags mask_extra_fields = 0x000f;
    // static const fmtflags gmt = 0x0010;
    // static const fmtflags local = 0x0020;
    // static const fmtflags clear = 0x0000;
    static bool has(fmtflags v) {
      if (v == fmtflags::clear) {
        return false;
      }
      if (v < fmtflags::mask_extra_fields) {
        auto x = (unsigned int) _flags & (unsigned int) fmtflags::mask_extra_fields;
        return ((fmtflags) x == v);
      }
      return (fmtflags) ((unsigned int) _flags & (unsigned int) v) == v;
    }
    static fmtflags flags() { return _flags; }
    static void reset() { _flags = static_cast<fmtflags>((unsigned int) fmtflags::gmt_or_local | (unsigned int) fmtflags::us); }
    static void set_flags(fmtflags v, bool on = true) {
      if (v == fmtflags::clear) {
        reset();
        return;
      }
      if (v < fmtflags::mask_extra_fields) {
        _flags = (fmtflags) ((unsigned int) _flags & ((unsigned int) fmtflags::mask_extra_fields + 1));
        _flags = (fmtflags) ((unsigned int) _flags | (unsigned int) (v));
        return;
      }

      if (on)
        _flags = (fmtflags) ((unsigned int) _flags | (unsigned int) (v));
      else
        _flags = (fmtflags) ((unsigned int) _flags & ~(unsigned int) (v));

      if (v == fmtflags::gmt)
        set_flags(fmtflags::gmt_or_local, true);
      else if (v == fmtflags::local)
        set_flags(fmtflags::gmt_or_local, false);
    }
    /**
     * @brief `saver` is a RAII class to simplify the save/restore of iom flags.
     *
     * @par For example:
     *
     * @code{c++}
     * {
     *     using iom_ = cmdr::chrono::iom;
     *     iom_::saver _iom_saver{}; // save point for 'iom'
     *     iom_::set_flags(iom_::fmtflags::gmt_or_local, false);
     *     std::stringstream ss;
     *     cmdr::chrono::serialize_time_point(ss, time, format);
     *     // restore point for 'iom' after _iom_saver dtor().
     * }
     * @endcode
     */
    class saver {
      fmtflags _flags;

    public:
      saver() { _flags = iom::flags(); }
      ~saver() { iom::set_as(_flags); }
    };

  private:
    static void set_as(fmtflags v) { _flags = v; }

  private:
    static fmtflags _flags; // 0:ms, 1:us, 2:ns
                            // static int gmt_or_local; //0:gmt, 1:local
  };

  inline iom::fmtflags iom::_flags = static_cast<fmtflags>((unsigned int) iom::fmtflags::gmt_or_local | (unsigned int) iom::fmtflags::us);

} // namespace cmdr::chrono

inline std::ostream &operator<<(std::ostream &os, const cmdr::chrono::iom::fmtflags v) {
  using iom = cmdr::chrono::iom;
  iom::set_flags(v);
  return os;
}

// format_duration, try_parse, duration_is_zero, ...
namespace cmdr::chrono {

  template<class Duration>
  inline std::ostream &format_duration_simple(std::ostream &os, Duration const &ns) {
    char fill = os.fill();
    os.fill('0');
#if __cplusplus > 201703L
    auto d = std::chrono::duration_cast<std::chrono::days>(ns);
#else
    typedef std::chrono::duration<long, std::ratio<86400>> days;
    auto d = std::chrono::duration_cast<days>(ns);
#endif
    ns -= d;
    auto h = std::chrono::duration_cast<std::chrono::hours>(ns);
    ns -= h;
    auto m = std::chrono::duration_cast<std::chrono::minutes>(ns);
    ns -= std::chrono::duration_cast<Duration>(m);
    auto s = std::chrono::duration_cast<std::chrono::seconds>(ns);
    // os << std::dec << std::noshowbase;
    if (d.count() > 0) os << /*std::setw(2) <<*/ d.count() << "d ";
    if (h.count() > 0) os << /*std::setw(2) <<*/ h.count() << "h";
    if (m.count() > 0) os << /*std::setw(2) <<*/ m.count() << "m";
    if (s.count() > 0) os << /*std::setw(2) <<*/ s.count() << 's';
    os.fill(fill);
    return os;
  }

  // inline std::ostream &format_duration_simple(std::ostream &os, std::chrono::duration<long double, std::ratio<60>> const &ns) {
  //     return os;
  // }
  // inline std::ostream &format_duration_simple(std::ostream &os, std::chrono::duration<long double, std::ratio<1>> const &ns) {
  //     return os;
  // }

  template<class Duration,
           std::enable_if_t<is_duration<Duration>::value, bool> = true>
  inline std::ostream &format_duration(std::ostream &os, Duration const &timeunit) {
    using namespace std;
    using namespace std::chrono;
    nanoseconds ns    = duration_cast<nanoseconds>(timeunit);
    // std::ostringstream os;
    bool foundNonZero = false;
    os.fill('0');
    typedef duration<int, std::ratio<86400 * 365>> years_;
    const auto y = duration_cast<years_>(ns);
    if (y.count()) {
      foundNonZero = true;
      os << y.count() << "y:";
      ns -= y;
    }
    typedef duration<int, std::ratio<86400>> days_;
    const auto d = duration_cast<days_>(ns);
    if (d.count()) {
      foundNonZero = true;
      os << d.count() << "d:";
      ns -= d;
    }
    const auto h = duration_cast<hours>(ns);
    if (h.count() || foundNonZero) {
      foundNonZero = true;
      os << h.count() << "h:";
      ns -= h;
    }
    const auto m = duration_cast<minutes>(ns);
    if (m.count() || foundNonZero) {
      foundNonZero = true;
      os << m.count() << "m";
      ns -= m;
    }

    bool z{};
    const auto s = duration_cast<seconds>(ns);
    if (s.count() || z) {
      z = true;
      if (foundNonZero) os << ':';
      os << s.count() << "s";
      ns -= s;
    }
    bool z1{};
    const auto ms = duration_cast<milliseconds>(ns);
    if (ms.count() || z1) {
      if (foundNonZero)
        os << std::setw(3) << ms.count();
      else
        os << ms.count();
      ns -= ms;
      z1 = true;
    }
    bool z2{}, zdot1{}, zdot2{};
    const auto us = duration_cast<microseconds>(ns);
    if (us.count() || z2) {
      if (z1) {
        os << '.';
        zdot1 = true;
      }
      if (foundNonZero)
        os << std::setw(3) << us.count();
      else
        os << us.count();
      ns -= us;
      z2 = true;
    }
    bool z3{};
    if (ns.count() || z3) {
      if (z1 || z2) {
        os << '.';
        zdot2 = true;
      }
      os << std::setw(3) << ns.count();
      z3 = true;
    }
    if (z1 || z2 || z3) {
      if (z3)
        os << (zdot1 ? "ms" : zdot2 ? "us"
                                    : "ns");
      else if (z2)
        os << (zdot1 ? "ms" : "us");
      else
        os << "ms";
    }
    return os; // .str();
  }

  template<class Duration,
           std::enable_if_t<is_duration<Duration>::value, bool> = true>
  inline std::string format_duration(Duration const &timeunit) {
    std::stringstream ss;
    format_duration(ss, timeunit);
    return ss.str();
  }


  //
  inline bool try_parse(std::tm &tm, const std::string &expression, const std::string &format) {
    std::stringstream ss(expression);
    return !(ss >> std::get_time(&tm, format.c_str())).fail();
  }


  template<typename Clock = std::chrono::system_clock>
  inline bool duration_is_zero(typename Clock::time_point const &tp) {
    auto d = tp.time_since_epoch();
    if (d != Clock::duration::zero())
      return false;
    return true;
  }


  template<class _Clock, class _Duration = typename _Clock::duration>
  inline auto time_point_get_ms(std::chrono::time_point<_Clock, _Duration> const &time) {
    using namespace std::chrono;
    milliseconds ms                = duration_cast<milliseconds>(time.time_since_epoch());
    // seconds s = duration_cast<seconds>(ms);
    // std::time_t t = s.count();
    std::size_t fractional_seconds = ms.count() % 1000;
    return fractional_seconds;
  }

  template<class _Clock, class _Duration = typename _Clock::duration>
  inline auto time_point_get_us(std::chrono::time_point<_Clock, _Duration> const &time) {
    using namespace std::chrono;
    microseconds us                = duration_cast<microseconds>(time.time_since_epoch());
    // seconds s = duration_cast<seconds>(ms);
    // std::time_t t = s.count();
    std::size_t fractional_seconds = us.count() % 1000;
    return fractional_seconds;
  }

  template<class _Clock, class _Duration = typename _Clock::duration>
  inline auto time_point_get_ns(std::chrono::time_point<_Clock, _Duration> const &time) {
    using namespace std::chrono;
    nanoseconds ns                 = duration_cast<nanoseconds>(time.time_since_epoch());
    // seconds s = duration_cast<seconds>(ms);
    // std::time_t t = s.count();
    std::size_t fractional_seconds = ns.count() % 1000;
    return fractional_seconds;
  }

  template<typename Clock = std::chrono::system_clock>
  inline typename Clock::time_point tm_2_time_point(std::tm *tm) {
    return Clock::from_time_t(std::mktime(tm));
  }

  template<typename Clock = std::chrono::system_clock>
  inline typename Clock::time_point tm_2_time_point(std::tm &tm) {
    return Clock::from_time_t(std::mktime(&tm));
  }

  template<typename Clock = std::chrono::system_clock, bool GMT = false>
  inline std::tm time_point_2_tm(typename Clock::time_point const tp) {
    auto time_now = Clock::to_time_t(tp);
    if (GMT) {
#if defined(_WIN32) || defined(_WIN64)
      struct tm tm1;
      errno_t err;
      err = _gmtime64_s(&tm1, &time_now);
      if (!err) {
        return tm1;
      }
      return tm1;
#else
      return *std::gmtime(&time_now);
#endif
    }
#if defined(_WIN32) || defined(_WIN64)
    std::tm tm1;
    localtime_s(&tm1, &time_now);
    return tm1;
#else
    return *std::localtime(&time_now);
#endif
  }

  template<typename Clock = std::chrono::system_clock, bool GMT = false>
  inline std::tm time_t_2_tm(time_t t) {
    if (GMT) {
      return *std::gmtime(&t);
    }
    return *std::localtime(&t);
  }

} // namespace cmdr::chrono

// serialize_time_point, serialize_tm, format_time_point, ...
namespace cmdr::chrono {
  // NOTE: just for std::chrono::system_clock
  template<class _Clock, class _Duration = typename _Clock::duration>
  inline std::ostream &serialize_time_point(std::ostream &os, std::chrono::time_point<_Clock, _Duration> const &time, const char *format = "%Y-%m-%d %H:%M:%S") {
    using iom_     = cmdr::chrono::iom;
    // using tp = std::chrono::time_point<_Clock, _Duration>;
    std::time_t tt = _Clock::to_time_t(time);
    if (tt == -1)
      return os;
    std::tm *tm{nullptr};
    if (iom_::has(iom_::fmtflags::gmt_or_local)) {
#if defined(_WIN32) || defined(_WIN64)
      struct tm tm1;
      __int64 ltime;
      errno_t err;
      _time64(&ltime);
      err = _gmtime64_s(&tm1, &ltime);
      if (!err) {
        // printf("Invalid Argument to _gmtime64_s.");
        tm = &tm1;
      }
#else
      tm = std::gmtime(&tt); // GMT (UTC)
#endif
    } else { // if (iom_::has(iom_::fmtflags::local))
#if defined(_WIN32) || defined(_WIN64)
      std::tm tm1;
      time_t now = time_t{0};
      localtime_s(&tm1, &now);
      tm = &tm1;
#else
      tm = std::localtime(&tt); // Locale time-zone, usually UTC by default.
#endif
    }
    // else
    //     tm = std::gmtime(&tt); //GMT (UTC)

    std::size_t ms = time_point_get_ms(time);
    if (iom_::has(iom_::fmtflags::ns)) {
      // auto t0 = std::chrono::high_resolution_clock::now();
      // auto nanosec = t0.time_since_epoch();

      std::size_t ns = time_point_get_ns(time);
      std::size_t us = time_point_get_us(time);
      // see also: `date -Ins` => 2021-08-05T11:46:39,911696444+01:00
      // another: `date +"%T.%9N"` => 11:49:19.162813535
      //
      os << std::put_time(tm, format) << ',' << std::setfill('0')
         << std::setw(3) << ms
         << std::setw(3) << us
         << std::setw(3) << ns
          // << ',' << std::setw(9) << nanosec.count()
          ;
    } else if (iom_::has(iom_::fmtflags::us)) {
      std::size_t fractional_seconds = time_point_get_us(time);
      os << std::put_time(tm, format) << '.' << std::setfill('0')
         << std::setw(3) << ms
         << std::setw(3) << fractional_seconds;
    } else if (iom_::has(iom_::fmtflags::ms)) {
      os << std::put_time(tm, format) << '.' << std::setfill('0')
         << std::setw(3) << ms;
    } else {
      os << std::put_time(tm, format);
    }

    return os;
  }
  template<class _Clock, class _Duration = typename _Clock::duration>
  inline std::string format_time_point(std::chrono::time_point<_Clock, _Duration> const &time, const char *format = "%Y-%m-%d %H:%M:%S") {
    std::stringstream ss;
    serialize_time_point(ss, time, format);
    return ss.str();
  }
  inline std::string format_time_point(const char *format = "%Y-%m-%d %H:%M:%S") { return format_time_point(std::chrono::system_clock::now(), format); }
  template<class _Clock, class _Duration = typename _Clock::duration>
  inline std::string format_time_point_to_local(std::chrono::time_point<_Clock, _Duration> const &time, const char *format = "%Y-%m-%d %H:%M:%S") {
    using iom_ = cmdr::chrono::iom;
    iom_::saver _iom_saver{};
    iom_::set_flags(iom_::fmtflags::gmt_or_local, false);
    std::stringstream ss;
    serialize_time_point(ss, time, format);
    return ss.str();
  }
  inline std::string format_time_point_to_local(const char *format = "%Y-%m-%d %H:%M:%S") { return format_time_point(std::chrono::system_clock::now(), format); }

  inline std::ostream &serialize_tm(std::ostream &os, std::tm const *tm, const char *format = "%Y-%m-%d %H:%M:%S") {
    os << std::put_time(tm, format);
    return os;
  }
  inline std::string format_tm(std::tm const *tm, const char *format = "%Y-%m-%d %H:%M:%S") {
    std::stringstream ss;
    ss << std::put_time(tm, format);
    return ss.str();
  }

  // inline std::string format(std::time_t time) {
  //     std::tm tm = *std::localtime(&time);
  // }
} // namespace cmdr::chrono

// last_day_at_this_month, last_day_at_this_year, compare_date_part
namespace cmdr::chrono {
  /**
   * @brief get last 'day_offset' day of this month.
   * @tparam Clock
   * @tparam GMT
   * @param tm
   * @param day_offset 1..31: get last day, others: invalid.
   * @param month_delta 1: this mo, 2: next mo, ...
   * @param mday the default is 1st (of next month)
   * @return new time point
   */
  template<typename Clock = std::chrono::system_clock, bool GMT = false>
  inline typename Clock::time_point last_day_at_this_month_in_time_point(std::tm const &tm, int day_offset = 1, int month_delta = 0, int mday = 1) {
    std::tm tmp = tm;
    if (day_offset < 1 || day_offset > 31)
      return tm_2_time_point(tmp);

    // get next month 1st
    tmp.tm_mday = mday; // get next month 1st day
    tmp.tm_mon += month_delta;
    while (tmp.tm_mon > 11) {
      tmp.tm_mon -= 12;
      tmp.tm_year++;
    }

    // subtract day_offset
    auto t = Clock::from_time_t(std::mktime(&tmp));
    // printf("  . . . . got next month 1st: %s\n", format_time_point(t).c_str());
    t -= std::chrono::hours((day_offset) * 24); // and subtract 1 day, so we get the last day in this month
    // printf("  . . . . got next month 1st - %d day: %s\n", day_offset, format_time_point(t).c_str());
    return t;
  }
  /**
   * @brief get last 'day_offset' day of this month.
   * @tparam Clock
   * @tparam GMT
   * @param tm
   * @param day_offset 1..31: get last day, others: invalid.
   * @param month_delta
   * @param mday the default is 1st (of next month)
   * @return new time point in std::tm structure
   */
  template<typename Clock = std::chrono::system_clock, bool GMT = false>
  inline std::tm last_day_at_this_month(std::tm const &tm, int day_offset = 1, int month_delta = 0, int mday = 1) {
    auto t = last_day_at_this_month_in_time_point(tm, day_offset, month_delta, mday);
    return time_point_2_tm(t);
  }

  /**
   * @brief get last 'day_offset' day of this year.
   * @tparam Clock
   * @tparam GMT
   * @param tm
   * @param day_offset 1..366: get last day, others: invalid.
   * @return new time point
   */
  template<typename Clock = std::chrono::system_clock, bool GMT = false>
  inline typename Clock::time_point last_day_at_this_year_in_time_point(std::tm const &tm, int day_offset = 1) {
    std::tm tmp = tm;
    if (day_offset < 1 || day_offset > 366)
      return tm_2_time_point(tmp);

    // get December 31st
    tmp.tm_mday = 1; // get next month 1st day
    tmp.tm_mon  = 0; // Dec 31th
    tmp.tm_year++;

    // subtract day_offset
    auto t = Clock::from_time_t(std::mktime(&tmp));
    // printf("  . . . . got December 31st: %s\n", format_time_point(t).c_str());
    t -= std::chrono::hours((day_offset) * 24); // and subtract 1 day, so we get the last day in this month
    // printf("  . . . . got December 31st + 1 - %d day: %s\n", day_offset, format_time_point(t).c_str());
    return t;
  }
  /**
   * @brief get last 'day_offset' day of this year.
   * @tparam Clock
   * @tparam GMT
   * @param tm
   * @param day_offset 1..366: get last day, others: invalid.
   * @return new time point in std::tm structure
   */
  template<typename Clock = std::chrono::system_clock, bool GMT = false>
  inline std::tm last_day_at_this_year(std::tm const &tm, int day_offset = 1) {
    auto t = last_day_at_this_year_in_time_point(tm, day_offset);
    return time_point_2_tm(t);
  }


  template<typename Clock = std::chrono::system_clock>
  inline bool compare_date_part(typename Clock::time_point const &lhs, typename Clock::time_point const &rhs) {
    std::tm tml = time_point_2_tm(lhs);
    std::tm tmr = time_point_2_tm(rhs);
    tml.tm_sec  = tmr.tm_sec;
    tml.tm_min  = tmr.tm_min;
    tml.tm_hour = tmr.tm_hour;
    auto l      = tm_2_time_point(tml);
    auto r      = tm_2_time_point(tmr);
    return l == r ? 0 : l < r ? -1
                              : 1;
  }

} // namespace cmdr::chrono

// clock class : inline functions
namespace cmdr::chrono {

  inline std::size_t clock::nsec() const { return time_point_get_ns(_now); }
  inline std::size_t clock::in_nsec() const {
    std::size_t ms = time_point_get_ms(_now);
    std::size_t us = time_point_get_us(_now);
    std::size_t ns = time_point_get_ns(_now);
    return (ms * 1000 + us) * 1000 + ns;
  }
  inline std::ostream &clock::serialize(std::ostream &os, const char *format) const {
    using iom_ = cmdr::chrono::iom;
    // using tp = std::chrono::time_point<_Clock, _Duration>;
    // std::time_t tt;
    std::tm *tm;
    if (iom_::has(iom_::fmtflags::gmt_or_local)) {
#if defined(_WIN32) || defined(_WIN64)
      struct tm tm1;
      __int64 ltime;
      errno_t err;
      _time64(&ltime);
      err = _gmtime64_s(&tm1, &ltime);
      if (!err) {
        tm = &tm1;
      } else {
        // printf("Invalid Argument to _gmtime64_s.");
        tm = &tm1;
      }
#else
      const auto tt = std::chrono::system_clock::to_time_t(_now);
      tm            = std::gmtime(&tt); // GMT (UTC)
#endif
    } else { // if (iom_::has(iom_::fmtflags::local))
#if defined(_WIN32) || defined(_WIN64)
      std::tm tm1;
      time_t now = time_t{0};
      localtime_s(&tm1, &now);
      tm = &tm1;
#else
      const auto tt = std::chrono::system_clock::to_time_t(_now);
      tm            = std::localtime(&tt); // Locale time-zone, usually UTC by default.
#endif
    }
    // else
    //     tm = std::gmtime(&tt); //GMT (UTC)

    if (iom_::has(iom_::fmtflags::ns)) {
      auto const _nsec = in_nsec();
      os << std::put_time(tm, format) << '.' << std::setfill('0')
         << std::setw(9) << _nsec;
    } else if (iom_::has(iom_::fmtflags::us)) {
      auto const _nsec = in_nsec();
      os << std::put_time(tm, format) << '.' << std::setfill('0')
         << std::setw(6) << (_nsec / 1000);
    } else if (iom_::has(iom_::fmtflags::ms)) {
      auto const _nsec = in_nsec();
      os << std::put_time(tm, format) << '.' << std::setfill('0')
         << std::setw(3) << (_nsec / 1'000'000);
    } else {
      os << std::put_time(tm, format);
    }

    return os;
  }
} // namespace cmdr::chrono

namespace cmdr::chrono {

  // other helpers


  /**
   * @brief parse a source string as a time structure with a list of formats.
   * @tparam _Args its type should be 'const char * const'
   * @param tm the parsed time value will be stored in it
   * @param source_string
   * @param formats lists of 'const char & const'
   * @return true means a time parsed ok, false means cannot be parsed.
   *
   * @detail For instance:
   * @code{c++}
   * std::tm tm;
   * auto time_str = "1937-1-29 3:59:59";
   * if (cmdr::chrono::try_parse_by(tm, time_str, "%H:%M:%S", "%Y-%m-%d %H:%M:%S", "%Y/%m/%d %H:%M:%S")) {
   *     auto tp = cmdr::chrono::tm_2_time_point(&tm);
   *     // ...
   * }
   * @endcode
   */
  template<typename... _Args>
  inline bool try_parse_by(std::tm &tm, std::string const &source_string, _Args const &...formats) {
    // if (sizeof...(_Args) > 0) {
    std::tm tm_local_copy = tm;
    for (auto &format: {"%Y-%m-%d %H:%M:%S", formats...}) {
      std::stringstream ss(source_string);
      if (!(ss >> std::get_time(&tm, format)).fail()) {
        // printf("  . . . . parsed: %s\n", format_time_point(now).c_str());
        return true;
      }
      tm = tm_local_copy;
    }
    // }
    return false;
  }

  template<typename Clock = std::chrono::system_clock, bool GMT = false>
  inline typename Clock::time_point parse_datetime(const char *str) {
    typename Clock::time_point n{};

    std::tm tm;
    if (GMT) {
#if defined(_WIN32) || defined(_WIN64)
      __int64 ltime;
      errno_t err;
      _time64(&ltime);
      err = _gmtime64_s(&tm, &ltime);
      // if (err) {
      //   // printf("Invalid Argument to _gmtime64_s.");
      // }
#else
      auto now      = Clock::now();
      auto time_now = Clock::to_time_t(now);
      tm            = *std::gmtime(&time_now);
#endif
    } else {
#if defined(_WIN32) || defined(_WIN64)
      time_t now = time_t{0};
      localtime_s(&tm, &now);
#else
      auto now      = Clock::now();
      auto time_now = Clock::to_time_t(now);
      tm            = *std::localtime(&time_now);
#endif
    }

    if (try_parse_by(tm, str,
                     "%Y-%m-%d", "%Y/%m/%d", "%H:%M:%S",
                     "%Y-%m-%d %H:%M:%S", "%Y/%m/%d %H:%M:%S")) {
      n = Clock::from_time_t(std::mktime(&tm));
      // printf("  . . . . parsed: %s\n", format_time_point(n).c_str());
    }

    return n;
  }


  // not yet
  template<class Duration,
           std::enable_if_t<is_duration<Duration>::value, bool> = true>
  static bool parse_duration(std::istream &is, Duration const &d) {
    (void) (is);
    (void) (d);
    return true;
  }

} // namespace cmdr::chrono


// friends

template<typename _Rep, typename _Period>
inline std::ostream &
operator<<(std::ostream &os,
           const std::chrono::duration<_Rep, _Period> &d) {
  return cmdr::chrono::format_duration(os, d);
}

template<typename T,
         std::enable_if_t<cmdr::chrono::is_duration<T>::value, bool>>
inline void cmdr::chrono::high_res_duration::print_duration(std::ostream &os, T const &v) {
#if defined(_WIN32)
  os << "It took ";
  cmdr::chrono::format_duration(os, v);
  os << '\n';
#else
  // cmdr::chrono::format_duration(os, v);
  os << "It took " << v << '\n';
#endif
}

/* namespace cmdr::checks {
  template<typename T>
  concept is_time_point = std::is_same_v<std::chrono::time_point<T, std::chrono::nanoseconds>, T>;
}

template<typename T>
  requires cmdr::checks::is_time_point<T>
inline std::ostream &operator<<(std::ostream &os, T const &time) {
  return cmdr::chrono::serialize_time_point(os, time, "[cool] %F %T");
} */

inline std::ostream &operator<<(std::ostream &os, std::chrono::system_clock::time_point const &time) {
  return cmdr::chrono::serialize_time_point(os, time, "%F %T");
}

// NOTE the time_t or time_point in a steady clock has no meaning.
//
//inline std::ostream &operator<<(std::ostream &os, std::chrono::steady_clock::time_point const &time) {
//  return cmdr::chrono::serialize_time_point(os, time, "%F %T");
//}

inline std::ostream &operator<<(std::ostream &os, std::tm const *tm) {
  return cmdr::chrono::serialize_tm(os, tm, "%F %T");
}
inline std::ostream &operator<<(std::ostream &os, std::tm const &tm) {
  return cmdr::chrono::serialize_tm(os, &tm, "%F %T");
}

inline std::ostream &operator<<(std::ostream &os, cmdr::chrono::clock const &v) {
  return v.serialize(os, "%F %T");
}


#endif // CMDR_CXX11_CMDR_CHRONO_HH
