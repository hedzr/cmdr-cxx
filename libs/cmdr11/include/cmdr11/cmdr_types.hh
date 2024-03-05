//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_TYPES_HH
#define CMDR_CXX11_CMDR_TYPES_HH

// BETTER in-place construction form without std::in_place_t tag.
//#define BETTER 1


#include <any>
#include <array>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


#include "cmdr_chrono.hh"
#include "cmdr_types_check.hh"


namespace cmdr {

  class obj {
  public:
    virtual ~obj() = default;
  };

  // old cmdr,
  using support_types = std::variant<std::monostate,
                                     bool, char,
                                     int, unsigned int, int8_t, int16_t,
                                     // int32_t, int64_t,
                                     uint8_t, uint16_t,
                                     long, long long, unsigned long, unsigned long long,
                                     float, double,
                                     std::chrono::nanoseconds,
                                     std::chrono::microseconds,
                                     std::chrono::milliseconds,
                                     std::chrono::seconds,
                                     std::chrono::minutes,
                                     std::chrono::hours,
#if __cplusplus > 201703L
                                     std::chrono::days,
                                     std::chrono::weeks,
                                     std::chrono::months,
                                     std::chrono::years,

                                     // std::chrono::time_zone,
                                     std::chrono::system_clock,
#endif
                                     std::byte,
                                     std::vector<std::string>,
                                     const char *,
                                     std::string>;

#if 0
    // from:
    //   https://stackoverflow.com/questions/42138599/how-to-format-stdchrono-durations
    //   http://coliru.stacked-crooked.com/a/26041d7303f8d343
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

    template<class... Durations>
    inline std::string format_durations(std::tuple<Durations...> d) {
        std::size_t values[] = {(std::size_t) std::get<Durations>(d).count()...};
        auto ratios = get_ratios<Durations...>();

        std::stringstream ss << std::setfill('0');
        ss << values[0];

        for (std::size_t const &v : values) {
            std::size_t i = &v - values;
            if (i == 0) continue;
            ss << "::" << std::setw(log_10_round_up(ratios[i - 1])) << values[i];
        }
        return ss.str();
    }
#endif


  // new cmdr,
  using base_type [[maybe_unused]] = std::any;
  // helper constant for the visitor #3
  template<class>
  [[maybe_unused]] inline constexpr bool always_false_v = false;

  [[maybe_unused]] inline std::string variant_to_string(const support_types &v) {
    std::stringstream ss;

    std::visit([&ss](auto &&arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, bool>)
        ss << arg;
      else if constexpr (std::is_same_v<T, char>)
        ss << arg;
      else if constexpr (std::is_same_v<T, int>)
        ss << arg;
      else if constexpr (std::is_same_v<T, int8_t>)
        ss << arg;
      else if constexpr (std::is_same_v<T, int16_t>)
        ss << arg;
      else if constexpr (std::is_same_v<T, int32_t>)
        ss << arg;
      else if constexpr (std::is_same_v<T, int64_t>)
        ss << arg;
      else if constexpr (std::is_same_v<T, unsigned int>)
        ss << arg;
      else if constexpr (std::is_same_v<T, uint8_t>)
        ss << arg;
      else if constexpr (std::is_same_v<T, uint16_t>)
        ss << arg;
      else if constexpr (std::is_same_v<T, uint32_t>)
        ss << arg;
      else if constexpr (std::is_same_v<T, uint64_t>)
        ss << arg;
      else if constexpr (std::is_same_v<T, long>)
        ss << arg;
      else if constexpr (std::is_same_v<T, long long>)
        ss << arg;
      else if constexpr (std::is_same_v<T, unsigned long>)
        ss << arg;
      else if constexpr (std::is_same_v<T, unsigned long long>)
        ss << arg;
      else if constexpr (std::is_same_v<T, float>)
        ss << arg;
      else if constexpr (std::is_same_v<T, double>)
        ss << arg;
      else if constexpr (std::is_same_v<T, std::string>)
        ss << std::quoted(arg);
      else if constexpr (std::is_same_v<T, std::chrono::nanoseconds>)
        chrono::format_duration(ss, arg);
      else if constexpr (std::is_same_v<T, std::chrono::microseconds>)
        chrono::format_duration(ss, arg);
      else if constexpr (std::is_same_v<T, std::chrono::milliseconds>)
        chrono::format_duration(ss, arg);
      else if constexpr (std::is_same_v<T, std::chrono::seconds>)
        chrono::format_duration(ss, arg);
      else if constexpr (std::is_same_v<T, std::chrono::minutes>)
        chrono::format_duration(ss, arg);
      else if constexpr (std::is_same_v<T, std::chrono::hours>)
        chrono::format_duration(ss, arg);
#if __cplusplus > 201703L
      else if constexpr (std::is_same_v<T, std::chrono::days>)
        chrono::format_duration(ss, arg);
      else if constexpr (std::is_same_v<T, std::chrono::weeks>)
        chrono::format_duration(ss, arg);
      else if constexpr (std::is_same_v<T, std::chrono::months>)
        chrono::format_duration(ss, arg);
      else if constexpr (std::is_same_v<T, std::chrono::years>)
        chrono::format_duration(ss, arg);
        // else if constexpr (std::is_same_v<T, std::chrono::system_clock>)
        //  chrono::format_duration(ss, arg);
#endif
      // else
      //     static_assert(always_false_v<T>, "non-exhaustive visitor!");
    },
               v);
    return ss.str();
  }
} // namespace cmdr


#endif // CMDR_CXX11_CMDR_TYPES_HH
