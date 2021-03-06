//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef CMDR_CXX11_CMDR_CHRONO_HH
#define CMDR_CXX11_CMDR_CHRONO_HH


#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include <functional>


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
                            decltype(std::declval<T>().zero()),
                            decltype(std::declval<T>().min()),
                            decltype(std::declval<T>().max())>,
                    void>> : public std::true_type {};


    //
    //
    //


    template<class... Durations, class DurationIn>
    std::tuple<Durations...> break_down_durations(DurationIn d) {
        std::tuple<Durations...> retval;
        using discard = int[];
        (void) discard{0, (void((
                                   (std::get<Durations>(retval) = std::chrono::duration_cast<Durations>(d)),
                                   (d -= std::chrono::duration_cast<DurationIn>(std::get<Durations>(retval))))),
                           0)...};
        return retval;
    }


    /**
     * @brief a high resolution time span calculator
     * 
     * Usage:
     * 
     *   Just make it as a stack variable, for example:
     * 
     *    void yours(){
     *          hicc::chrono::high_res_duration hrd;
     *          
     *          //...
     *          
     *          // at the exiting this function, hrd will print a timing log line.
     *    }
     */
    class high_res_duration {
    public:
        high_res_duration(std::function<void(std::chrono::high_resolution_clock::duration duration)> const &fn = nullptr)
            : _then(std::chrono::high_resolution_clock::now())
            , _cb(fn) {}
        ~high_res_duration() {
            _now = std::chrono::high_resolution_clock::now();
            auto duration = _now - _then;

            // auto [ss, ms, us] = break_down_durations<std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds>(duration);

            auto clean_duration = break_down_durations<std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds>(duration);
            //    auto timeInMicroSec = std::chrono::duration_cast<std::chrono::microseconds>(duration); // base in Microsec.
            std::cout << std::get<0>(clean_duration).count() << "::" << std::get<1>(clean_duration).count() << "::" << std::get<2>(clean_duration).count() << "\n";
            if (_cb)
                _cb(duration);
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> _then, _now;
        std::function<void(std::chrono::high_resolution_clock::duration)> _cb;
    };


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

    template<typename T>
    inline std::ostream &format_duration(std::ostream &os, T timeunit) {
        using namespace std;
        using namespace std::chrono;
        nanoseconds ns = duration_cast<nanoseconds>(timeunit);
        // std::ostringstream os;
        bool foundNonZero = false;
        os.fill('0');
        typedef duration<int, std::ratio<86400 * 365>> years;
        const auto y = duration_cast<years>(ns);
        if (y.count()) {
            foundNonZero = true;
            os << y.count() << "y:";
            ns -= y;
        }
        typedef duration<int, std::ratio<86400>> days;
        const auto d = duration_cast<days>(ns);
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
            if (foundNonZero) {
                os << std::setw(3);
            }
            os << ms.count();
            ns -= ms;
            z1 = true;
        }
        bool z2{};
        const auto us = duration_cast<microseconds>(ns);
        if (us.count() || z2) {
            if (foundNonZero) {
                os << std::setw(3);
            }
            if (z1) os << '.';
            os << us.count();
            ns -= us;
            z2 = true;
        }
        bool z3{};
        if (ns.count() || z3) {
            if (z1 || z2) os << '.';
            os << std::setw(3) << ns.count();
            z3 = true;
        }
        if (z1 || z2 || z3) {
            if (z3)
                os << "ns";
            else if (z2)
                os << "us";
            else
                os << "ms";
        }
        return os; // .str();
    }


    template<class Duration,
             std::enable_if_t<is_duration<Duration>::value, bool> = true>
    static bool parse_duration(std::istream &is, Duration &d) {
        (void) (is);
        (void) (d);
        return true;
    }


} // namespace cmdr::chrono

template<typename T,
         std::enable_if_t<cmdr::chrono::is_duration<T>::value, bool> = true>
inline std::ostream &operator<<(std::ostream &os, T const &v) {
    cmdr::chrono::format_duration(os, v);
    return os;
}

#endif //CMDR_CXX11_CMDR_CHRONO_HH
