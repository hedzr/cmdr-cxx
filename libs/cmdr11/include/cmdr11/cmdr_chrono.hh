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


namespace cmdr::chrono {


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

    class high_res_duration {
    public:
        high_res_duration()
            : _then(std::chrono::high_resolution_clock::now()) {}
        ~high_res_duration() {
            _now = std::chrono::high_resolution_clock::now();
            auto duration = _now - _then;

            // auto [ss, ms, us] = break_down_durations<std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds>(duration);

            auto clean_duration = break_down_durations<std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds>(duration);
            //    auto timeInMicroSec = std::chrono::duration_cast<std::chrono::microseconds>(duration); // base in Microsec.
            std::cout << std::get<0>(clean_duration).count() << "::" << std::get<1>(clean_duration).count() << "::" << std::get<2>(clean_duration).count() << "\n";
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> _then, _now;
    };


    template<class Duration>
    inline std::ostream &format_duration(std::ostream &os, Duration ns) {
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

} // namespace cmdr::chrono

#endif //CMDR_CXX11_CMDR_CHRONO_HH
