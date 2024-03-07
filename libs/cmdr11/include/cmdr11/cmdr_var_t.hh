//
// Created by Hedzr Yeh on 2021/1/16.
//

#ifndef CMDR_CXX11_CMDR_VAR_T_HH
#define CMDR_CXX11_CMDR_VAR_T_HH

#include <any>
#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <type_traits>
#include <typeindex>

#include <utility>

#include <cmath>
#include <complex>

#include <mutex>
#include <shared_mutex>
#include <thread>


#include "cmdr_defs.hh"
#include "cmdr_types.hh"
#include "cmdr_types_check.hh"

#include "cmdr_dbg.hh"
#include "cmdr_log.hh"

#include "cmdr_chrono.hh"
#include "cmdr_string.hh"
#include "cmdr_terminal.hh"


namespace cmdr::vars {


  namespace detail {
    template<class T, class F>
    inline std::pair<const std::type_index, std::function<void(std::ostream &os, std::any const &)>>
    to_any_visitor(F const &f) {
      return {
          std::type_index(typeid(T)),
          [g = f](std::ostream &os, std::any const &a) {
            UNUSED(a);
            if constexpr (std::is_void_v<T>)
              g(os);
            else
              g(os, std::any_cast<T const &>(a));
          }};
    }
    template<class T, class F>
    inline std::pair<const std::type_index, std::function<void(std::istream &is, std::any &)>>
    from_any_visitor(F const &f) {
      return {
          std::type_index(typeid(T)),
          [g = f](std::istream &is, std::any &a) {
            UNUSED(a);
            if constexpr (std::is_void_v<T>) {
              g(is);
            } else if constexpr (std::is_same_v<typename debug::lambda_func_type<decltype(g)>::type, void(std::istream &, std::any &) const>) {
              g(is, a);
            } else {
              T t{};
              g(is, t);
              a.template emplace<T>(t);
            }
          }};
    }
  } // namespace detail


  class variable {
  public:
    typedef variable self_type;
    using target_type = std::any;

    variable()        = default;
    template<class A, typename... Args,
             std::enable_if_t<
#if __GNUC__ > 9
                 std::is_constructible<target_type, A, Args...>::value &&
#endif
                     //! std::is_same<std::decay_t<A>, variable>::value &&
                     !std::is_same<std::decay_t<A>, std::any>::value &&
                     !std::is_same<std::decay_t<A>, self_type>::value,
                 int> = 0>
    explicit variable(A &&a0, Args &&...args)
        : _value(std::forward<A>(a0), std::forward<Args>(args)...) {
    }
    template<class A,
             std::enable_if_t<
#if __GNUC__ > 9
                 std::is_constructible<target_type, A>::value &&
#endif
                     //! std::is_same<std::decay_t<A>, variable>::value &&
                     !std::is_same<std::decay_t<A>, std::any>::value &&
                     !std::is_same<std::decay_t<A>, self_type>::value,
                 int> = 0>
    explicit variable(A &&a)
        : _value(std::forward<A>(a)) {
    }
    explicit variable(std::any &&a)
        : _value(std::move(a)) {}
    explicit variable(std::any a)
        : _value(std::move(a)) {}
    // explicit variable(variable const &a)
    //     : _value(a._value) {}
    // explicit variable(variable &&a)
    //     : _value(std::move(a._value)) {}
    virtual ~variable() = default;

    template<typename T>
    variable &operator=(T const &o) {
      _value = o;
      return (*this);
    }
    template<typename T>
    variable &operator=(T &&o) {
      _value = std::move(o);
      return (*this);
    }

    void clone_to(variable &target) const { target._copy(*this); }
    void _copy(variable const &o) {
      _value.operator=(o._value);
    }

    // variable& operator=(const variable& o){
    //     (*this)._value.swap(o.value_any());
    // }

    template<class... Args>
    void emplace(Args &&...args) {
      _value.template emplace<Args...>(args...);
      // auto v = {args...};
      // (void) v;
    }
    void emplace(std::any &&o) { _value.swap(o); }
    void emplace(std::any const &o) { _value = o; }

    [[nodiscard]] target_type const &underlying_value() const { return _value; }
    target_type &underlying_value() { return _value; }

    [[nodiscard]] std::string as_string() const {
      std::stringstream os;
      os << (*this);
      return os.str();
    }

#if defined(_DEBUG)
    /**
     * @brief as() will report more error detail while type error occurred for debugging.
     * @tparam T
     * @return
     */
    template<class T>
    T as() {
      return cast_as<T>();
    }
    template<class T>
    T const as() const {
      return cast_as<T>();
    }
#else
    template<class T>
    T as() {
      return std::any_cast<T>(_value);
    }
    template<class T>
    T const as() const {
      return std::any_cast<T>(_value);
    }
#endif

    template<class T>
    T cast_as() {
      try {
        return std::any_cast<T>(_value);
      } catch (std::bad_cast const &e) {
        char buf[512];
        std::ostringstream vs;
        vs << (*this);
        std::snprintf(buf, sizeof(buf), "can't cast type '%s' (value: '%s') -> type '%s', (ex: %s)",
                     _value.type().name(),
                     vs.str().c_str(),
                     debug::type_name<T>().data(),
                     e.what());
        cmdr_throw_line(buf);
      }
    }
    template<class T>
    T const cast_as() const {
      try {
        return std::any_cast<T>(_value);
      } catch (std::bad_cast const &e) {
        char buf[512];
        std::ostringstream vs;
        vs << (*this);
        std::snprintf(buf, sizeof(buf), "can't cast type '%s' (value: '%s') -> type '%s', (ex: %s)",
                     _value.type().name(),
                     vs.str().c_str(),
                     debug::type_name<T>().data(),
                     e.what());
        cmdr_throw_line(buf);
      }
    }


    [[nodiscard]] bool has_value() const { return _value.has_value(); }
    [[nodiscard]] bool empty() const { return !_value.has_value(); }
    [[nodiscard]] const std::type_info &type() const noexcept { return _value.type(); }
    void reset() { _value.reset(); }

  public:
    static variable parse(std::string &s);

  protected:
    template<class T>
    static void format_array(std::ostream &os, std::vector<T> const &o) {
      int i = 0;
      os << '[';
      for (typename std::vector<T>::const_iterator it = o.begin(); it != o.end(); it++) {
        if (i++)
          os << ',';
        os << (*it);
      }
      os << ']';
    }

    template<class T>
    static void format_list(std::ostream &os, std::list<T> const &o) {
      int i = 0;
      os << '[';
      for (typename std::vector<T>::const_iterator it = o.begin(); it != o.end(); it++) {
        if (i++)
          os << ',';
        os << (*it);
      }
      os << ']';
    }

    template<class T>
    static void format_complex(std::ostream &os, std::complex<T> const &o) {
      // os << o;
      auto r = o.real();
      if (r != 0) {
        os << r;
      }
      auto i = o.imag();
      if (i != 0) {
        if (r != 0)
          os << '+';
        os << i << 'i';
      }
    }

    template<class T>
    static bool parse_complex(std::istream &is, std::complex<T> &a) {
      bool ok;
      std::string s;
      ok = string::read_until(is, s, "+");
      {
        std::stringstream ss(s);
        T t;
        ss >> t;
        a.real(t);
      }
      if (ok) {
        ok = string::read_until(is, s, "i");
        std::stringstream ss(s);
        T t;
        ss >> t;
        a.imag(t);
      }
      return true;
    }

    template<class T>
    static bool parse_array(std::istream &is, std::vector<T> &a) {
      bool ok;
      char ch;
      std::string s;
      is >> ch;
      if (ch != '[') s += ch;
      while ((ok = string::read_until(is, s, ",")) == true) {
        std::stringstream ss(s);
        T t;
        ss >> t;
        a.push_back(t);
      }

      std::getline(is, s);
      s = string::strip(s, "", "]");
      T t;
      std::stringstream ss(s);
      ss >> t;
      a.push_back(t);
      return true;
    }

    static bool parse_void(std::istream &is) {
      std::string tmp;
      if (is.peek() == '{') {
        string::read_until(is, "{}");
      }
      return true;
    }

    /**
     * @brief 1/0, T/F, Y/N, True/False, Yes/No, On/Off
     * @param is
     * @param x
     * @return
     */
    static bool parse_bool(std::istream &is, bool &x) {
      int ch;
      ch = is.peek();
      if (ch == '0') {
        x = false;
        is >> ch;
        return true;
      }
      if (ch == '1') {
        x = true;
        is >> ch;
        return true;
      }

      if (ch == 't' || ch == 'T') {
        is >> ch;
        ch = is.peek();
        if (ch == 'r' || ch == 'R') {
          is >> ch;
          ch = is.peek();
          if (ch == 'u' || ch == 'U') {
            is >> ch;
            ch = is.peek();
            if (ch == 'e' || ch == 'E') {
              is >> ch;
              x = true;
              return true;
            }
            is.unget();
          }
          is.unget();
        } else {
          x = true;
          return true;
        }
        is.unget();
      } else if (ch == 'y' || ch == 'Y') {
        is >> ch;
        is >> ch;
        ch = is.peek();
        if (ch == 'e' || ch == 'E') {
          is >> ch;
          ch = is.peek();
          if (ch == 's' || ch == 'S') {
            is >> ch;
            x = true;
            return true;
          }
          is.unget();
        } else {
          x = true;
          return true;
        }
        is.unget();
      } else if (ch == 'f' || ch == 'F') {
        is >> ch;
        ch = is.peek();
        if (ch == 'a' || ch == 'A') {
          is >> ch;
          ch = is.peek();
          if (ch == 'l' || ch == 'L') {
            is >> ch;
            ch = is.peek();
            if (ch == 's' || ch == 'S') {
              is >> ch;
              ch = is.peek();
              if (ch == 'e' || ch == 'E') {
                is >> ch;
                x = false;
                return true;
              }
              is.unget();
            }
            is.unget();
          }
          is.unget();
        } else {
          x = false;
          return true;
        }
        is.unget();
      } else if (ch == 'n' || ch == 'N') {
        is >> ch;
        is >> ch;
        ch = is.peek();
        if (ch == 'o' || ch == 'O') {
          is >> ch;
          x = false;
          return true;
        }
        x = false;
        return true;

      } else if (ch == 'o' || ch == 'O') {
        is >> ch;
        ch = is.peek();
        if (ch == 'n' || ch == 'N') {
          is >> ch;
          x = true;
          return true;
        }
        if (ch == 'f' || ch == 'F') {
          is >> ch;
          ch = is.peek();
          if (ch == 'f' || ch == 'F') {
            is >> ch;
            x = false;
            return true;
          }
          is.unget();
        }
        is.unget();
      }
      return false;
    }

  private:
    typedef std::unordered_map<std::type_index, std::function<void(std::istream &is, std::any &)>> I;
    static I &any_parsers() {
      static I _registry {
        detail::from_any_visitor<void>([](std::istream &is) { parse_void(is); }),
            detail::from_any_visitor<bool>([](std::istream &is, bool &x) { parse_bool(is, x); }),
            detail::from_any_visitor<int>([](std::istream &is, int &x) { is >> x; }),
            detail::from_any_visitor<int8_t>([](std::istream &is, int8_t &x) { is >> x; }),
            detail::from_any_visitor<int16_t>([](std::istream &is, int16_t &x) { is >> x; }),
            detail::from_any_visitor<int32_t>([](std::istream &is, int32_t &x) { is >> x; }),
            detail::from_any_visitor<int64_t>([](std::istream &is, int64_t &x) { is >> x; }),
            detail::from_any_visitor<unsigned>([](std::istream &is, unsigned &x) { is >> x; }),
            detail::from_any_visitor<uint8_t>([](std::istream &is, uint8_t &x) { is >> x; }),
            detail::from_any_visitor<uint16_t>([](std::istream &is, uint16_t &x) { is >> x; }),
            detail::from_any_visitor<uint32_t>([](std::istream &is, uint32_t &x) { is >> x; }),
            detail::from_any_visitor<uint64_t>([](std::istream &is, uint64_t &x) { is >> x; }),
            detail::from_any_visitor<long>([](std::istream &is, long &x) { is >> x; }),
            detail::from_any_visitor<unsigned long>([](std::istream &is, unsigned long &x) { is >> x; }),
            detail::from_any_visitor<long long>([](std::istream &is, long long &x) { is >> x; }),
            detail::from_any_visitor<unsigned long long>([](std::istream &is, unsigned long long &x) { is >> x; }),
            detail::from_any_visitor<float>([](std::istream &is, float &x) { is >> x; }),
            detail::from_any_visitor<double>([](std::istream &is, double &x) { is >> x; }),
            detail::from_any_visitor<long double>([](std::istream &is, long double &x) { is >> x; }),
            detail::from_any_visitor<char const *>([](std::istream &is, std::any &a) { std::string s; string::strip_quotes(is, s); a=s; }),
            detail::from_any_visitor<std::string>([](std::istream &is, std::string &s) { string::strip_quotes(is, s); }),
#if __clang__
            detail::from_any_visitor<variable>([](std::istream &is, variable &x) { is >> x; }),
#endif

            detail::from_any_visitor<std::vector<char const *>>([](std::istream &is, std::any &a) { std::vector<std::string> vec; parse_array(is, vec);a=vec; }),
            detail::from_any_visitor<std::vector<std::string>>([](std::istream &is, std::vector<std::string> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<bool>>([](std::istream &is, std::vector<bool> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<int>>([](std::istream &is, std::vector<int> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<int8_t>>([](std::istream &is, std::vector<int8_t> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<int16_t>>([](std::istream &is, std::vector<int16_t> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<int32_t>>([](std::istream &is, std::vector<int32_t> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<int64_t>>([](std::istream &is, std::vector<int64_t> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<unsigned>>([](std::istream &is, std::vector<unsigned> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<uint8_t>>([](std::istream &is, std::vector<uint8_t> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<uint16_t>>([](std::istream &is, std::vector<uint16_t> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<uint32_t>>([](std::istream &is, std::vector<uint32_t> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<uint64_t>>([](std::istream &is, std::vector<uint64_t> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<long>>([](std::istream &is, std::vector<long> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<unsigned long>>([](std::istream &is, std::vector<unsigned long> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<long long>>([](std::istream &is, std::vector<long long> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<unsigned long long>>([](std::istream &is, std::vector<unsigned long long> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<float>>([](std::istream &is, std::vector<float> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<double>>([](std::istream &is, std::vector<double> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<long double>>([](std::istream &is, std::vector<long double> &a) { parse_array(is, a); }),
            detail::from_any_visitor<std::vector<variable>>([](std::istream &is, std::vector<variable> &a) { parse_array(is, a); }),

            detail::from_any_visitor<std::complex<float>>([](std::istream &is, std::complex<float> &a) { parse_complex(is, a); }),
            detail::from_any_visitor<std::complex<double>>([](std::istream &is, std::complex<double> &a) { parse_complex(is, a); }),
            detail::from_any_visitor<std::complex<long double>>([](std::istream &is, std::complex<long double> &a) { parse_complex(is, a); }),

            detail::from_any_visitor<std::chrono::nanoseconds>([](std::istream &is, std::chrono::nanoseconds &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::microseconds>([](std::istream &is, std::chrono::microseconds &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::milliseconds>([](std::istream &is, std::chrono::milliseconds &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::seconds>([](std::istream &is, std::chrono::seconds &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::minutes>([](std::istream &is, std::chrono::minutes &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::hours>([](std::istream &is, std::chrono::hours &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::duration<long double, std::ratio<1>>>([](std::istream &is, std::chrono::duration<long double, std::ratio<1>> &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::duration<long double, std::ratio<60>>>([](std::istream &is, std::chrono::duration<long double, std::ratio<60>> &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::duration<double, std::ratio<60>>>([](std::istream &is, std::chrono::duration<double, std::ratio<60>> &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::duration<float, std::ratio<60>>>([](std::istream &is, std::chrono::duration<float, std::ratio<60>> &x) { cmdr::chrono::parse_duration(is, x); }),
            detail::from_any_visitor<std::chrono::duration<float, std::ratio<1>>>([](std::istream &is, std::chrono::duration<float, std::ratio<1>> &x) { cmdr::chrono::parse_duration(is, x); }),
      };
      return _registry;
    }
    typedef std::unordered_map<std::type_index, std::function<void(std::ostream &os, std::any const &)>> R;
    static R &any_visitors() {
      static R _registry{
          detail::to_any_visitor<void>([](std::ostream &os) { os << "{}"; }),
          detail::to_any_visitor<bool>([](std::ostream &os, bool x) { os << std::boolalpha << x; }),
          detail::to_any_visitor<int>([](std::ostream &os, int x) { os << x; }),
          detail::to_any_visitor<int8_t>([](std::ostream &os, int8_t x) { os << x; }),
          detail::to_any_visitor<int16_t>([](std::ostream &os, int16_t x) { os << x; }),
          detail::to_any_visitor<int32_t>([](std::ostream &os, int32_t x) { os << x; }),
          detail::to_any_visitor<int64_t>([](std::ostream &os, int64_t x) { os << x; }),
          detail::to_any_visitor<unsigned>([](std::ostream &os, unsigned x) { os << x; }),
          detail::to_any_visitor<uint8_t>([](std::ostream &os, uint8_t x) { os << x; }),
          detail::to_any_visitor<uint16_t>([](std::ostream &os, uint16_t x) { os << x; }),
          detail::to_any_visitor<uint32_t>([](std::ostream &os, uint32_t x) { os << x; }),
          detail::to_any_visitor<uint64_t>([](std::ostream &os, uint64_t x) { os << x; }),
          detail::to_any_visitor<long>([](std::ostream &os, long x) { os << x; }),
          detail::to_any_visitor<unsigned long>([](std::ostream &os, unsigned long x) { os << x; }),
          detail::to_any_visitor<long long>([](std::ostream &os, long long x) { os << x; }),
          detail::to_any_visitor<unsigned long long>([](std::ostream &os, unsigned long long x) { os << x; }),
          detail::to_any_visitor<float>([](std::ostream &os, float x) { os << x; }),
          detail::to_any_visitor<double>([](std::ostream &os, double x) { os << x; }),
          detail::to_any_visitor<long double>([](std::ostream &os, long double x) { os << x; }),
          detail::to_any_visitor<char const *>([](std::ostream &os, char const *s) { os << std::quoted(s); }),
          detail::to_any_visitor<std::string>([](std::ostream &os, std::string const &s) { os << std::quoted(s); }),
          detail::to_any_visitor<variable>([](std::ostream &os, variable const &x) { os << x; }),

          detail::to_any_visitor<std::vector<char const *>>([](std::ostream &os, std::vector<char const *> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<std::string>>([](std::ostream &os, std::vector<std::string> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<bool>>([](std::ostream &os, std::vector<bool> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<int>>([](std::ostream &os, std::vector<int> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<int8_t>>([](std::ostream &os, std::vector<int8_t> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<int16_t>>([](std::ostream &os, std::vector<int16_t> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<int32_t>>([](std::ostream &os, std::vector<int32_t> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<int64_t>>([](std::ostream &os, std::vector<int64_t> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<unsigned>>([](std::ostream &os, std::vector<unsigned> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<uint8_t>>([](std::ostream &os, std::vector<uint8_t> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<uint16_t>>([](std::ostream &os, std::vector<uint16_t> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<uint32_t>>([](std::ostream &os, std::vector<uint32_t> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<uint64_t>>([](std::ostream &os, std::vector<uint64_t> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<long>>([](std::ostream &os, std::vector<long> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<unsigned long>>([](std::ostream &os, std::vector<unsigned long> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<long long>>([](std::ostream &os, std::vector<long long> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<unsigned long long>>([](std::ostream &os, std::vector<unsigned long long> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<float>>([](std::ostream &os, std::vector<float> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<double>>([](std::ostream &os, std::vector<double> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<long double>>([](std::ostream &os, std::vector<long double> const &a) { format_array(os, a); }),
          detail::to_any_visitor<std::vector<variable>>([](std::ostream &os, std::vector<variable> const &a) { format_array(os, a); }),

          detail::to_any_visitor<std::complex<float>>([](std::ostream &os, std::complex<float> const &a) { format_complex(os, a); }),
          detail::to_any_visitor<std::complex<double>>([](std::ostream &os, std::complex<double> const &a) { format_complex(os, a); }),
          detail::to_any_visitor<std::complex<long double>>([](std::ostream &os, std::complex<long double> const &a) { format_complex(os, a); }),

          detail::to_any_visitor<std::chrono::nanoseconds>([](std::ostream &os, const std::chrono::nanoseconds &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::microseconds>([](std::ostream &os, const std::chrono::microseconds &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::milliseconds>([](std::ostream &os, const std::chrono::milliseconds &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::seconds>([](std::ostream &os, const std::chrono::seconds &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::minutes>([](std::ostream &os, const std::chrono::minutes &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::hours>([](std::ostream &os, const std::chrono::hours &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::duration<long double, std::ratio<1>>>([](std::ostream &os, const std::chrono::duration<long double, std::ratio<1>> &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::duration<long double, std::ratio<60>>>([](std::ostream &os, const std::chrono::duration<long double, std::ratio<60>> &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::duration<double, std::ratio<60>>>([](std::ostream &os, const std::chrono::duration<double, std::ratio<60>> &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::duration<float, std::ratio<60>>>([](std::ostream &os, const std::chrono::duration<float, std::ratio<60>> &x) { cmdr::chrono::format_duration(os, x); }),
          detail::to_any_visitor<std::chrono::duration<float, std::ratio<1>>>([](std::ostream &os, const std::chrono::duration<float, std::ratio<1>> &x) { cmdr::chrono::format_duration(os, x); }),
          // ... add more handlers for your types ...
      };
      return _registry;
    }

    static inline bool process(std::istream &is, std::any &a) {
      auto &reg = any_parsers();
      if (const auto it = reg.find(std::type_index(a.type()));
          it != reg.cend()) {
        it->second(is, a);
        return true;
      }
#if defined(_DEBUG)
      std::cout << "Unregistered type for parsing " << std::quoted(a.type().name()) << "" << '\n'; // ︎︎︎\︎\ u fe0e  // ◌︎ // Variation Selector-15 (VS15)
      std::cerr << "Unregistered type for parsing " << std::quoted(a.type().name()) << "" << '\n'; // U+FE0E
#endif
      return false;
    }
    static inline bool process(std::ostream &os, std::any const &a) {
      auto &reg = any_visitors();
      if (const auto it = reg.find(std::type_index(a.type()));
          it != reg.cend()) {
        auto op = it->second;
        op(os, a);
        return true;
      }
#if defined(_DEBUG)
      std::cout << "Unregistered type for visiting " << std::quoted(a.type().name()) << "" << '\n'; // U+FE0E
      std::cerr << "Unregistered type for visiting " << std::quoted(a.type().name()) << "" << '\n'; // U+FE0E
#endif
      return false;
    }

    template<class T, class F>
    inline void register_visitor(F const &f) {
      std::cout << "Register visitor for type "
                << std::quoted(typeid(T).name()) << '\n';
      any_visitors().insert(detail::to_any_visitor<T>(f));
    }

    friend std::ostream &operator<<(std::ostream &os, variable const &o) {
      process(os, o._value);
      return os;
    }
    /**
     * @brief convert to a value from stream, and put it into variable
     * @param is
     * @param o
     * @return
     *
     * For an exact converting, A responding initial value with right type
     * must be set into variable. For example:
     *
     *     std::istringstream ss("true");
     *     cmdr::vars::variable v{false}; // or v{} to initialize a bool value implicitly.
     *     ss >> v; // so that we can convert the input stream with that type.
     *     std::cout << std::boolalpha << v; // print v will get this output: true.
     *
     */
    friend std::istream &operator>>(std::istream &is, variable &o) {
      process(is, o._value);
      return is;
    }

  private:
    target_type _value;
  }; // class variable

  // variable:
  //


  // forward

  template<class T = variable, typename small_string = std::string>
  class treeT;

  template<class T = variable, class small_string = std::string>
  class nodeT;

  using tcolorize = cmdr::terminal::colors::colorize;


  /**
   * @brief wrap type T as a node for treeT<T,...>
   * @tparam T must implement clone_to(T&), variadic template constructors, operator=, operator>>, and operator<<, etc..
   * @tparam small_string
   */
  template<class T, class small_string>
  class nodeT : public T {
  public:
    friend class treeT<T, small_string>;

    // typedef nodeT<T, small_string> self_type;
    using self_type    = nodeT;
    using parent_type  = T;
    using node_pointer = std::shared_ptr<self_type>;
    using key_type     = small_string;
    using node_vec     = std::list<node_pointer>;
    using node_map     = std::unordered_map<key_type, node_pointer>;
    using node_idx     = std::unordered_map<key_type, node_pointer>;

    nodeT()            = default;
    explicit nodeT(self_type const &o)
        : T{} {
      _parent   = o._parent;
      _indexes  = o._indexes;
      _children = o._children;
      o.clone_to(*this);
    }
    template<class A, typename... Args,
             std::enable_if_t<
                 std::is_constructible<T, A, Args...>::value &&
                     !std::is_same<std::decay_t<A>, self_type>::value,
                 int> = 0>
    explicit nodeT(A &&a0, Args &&...args)
        : T(std::forward<A>(a0), std::forward<Args>(args)...) {}
    template<class A,
             std::enable_if_t<
                 std::is_constructible<T, A>::value &&
                     !std::is_same<std::decay_t<A>, self_type>::value,
                 int> = 0>
    explicit nodeT(A &&a)
        : T(std::forward<A>(a)) {}
    ~nodeT() override = default;

  public:
    static bool dump_with_type_name;

    void reset() {
      std::unique_lock<std::shared_mutex> lock(mutex_);
      _children.clear();
      _indexes.clear();
      _parent = nullptr;
    }

  private:
    node_map _children;
    node_idx _indexes;
    node_pointer _parent{};
    mutable std::shared_mutex mutex_{};

  public:
    template<class A, typename... Args,
             std::enable_if_t<
                 std::is_constructible<T, A, Args...>::value &&
                     !std::is_same<std::decay_t<A>, self_type>::value,
                 int> = 0>
    void set(char const *key, A &&a0, Args &&...args) {
#if defined(CMDR_ENABLE_VERBOSE_LOG)
      // std::ostringstream osdbg;
      // osdbg << a0;
      cmdr_verbose_debug("     > [store] _set '%s' => '?', ... (variadic)", key);
#endif
      this->_set(
          key, [&](char const *, node_pointer) {}, a0, args...);
    }
    template<class A,
             std::enable_if_t<std::is_constructible<T, A>::value &&
                                  !std::is_same<std::decay_t<A>, self_type>::value,
                              int> = 0>
    void set(char const *key, A &&a) {
#if defined(CMDR_ENABLE_VERBOSE_LOG)
      // std::ostringstream osdbg;
      // osdbg << a;
      cmdr_verbose_debug("     > [store] _set '%s' => '?'", key);
#endif
      this->_set(
          key, [&](char const *, node_pointer) {}, a);
    }

  private:
    typedef std::function<void(char const *, node_pointer)> update_parent_index;

    template<class A, typename... Args,
             std::enable_if_t<
                 std::is_constructible<T, A, Args...>::value &&
                     !std::is_same<std::decay_t<A>, self_type>::value,
                 int> = 0>
    void _set(char const *key, update_parent_index const &cb, A &&a0, Args &&...args) {
      char const *part = std::strchr(key, '.');
      if (part) {
        small_string base(key);
        std::size_t pos = part - key;
        small_string k = base.substr(0, pos), remains = base.substr(pos + 1);
        auto it = _children.find(k);
        if (it == _children.end()) {
          _children.emplace(std::make_pair(k, std::make_shared<self_type>()));
          it = _children.find(k);
        }
        it->second->_set(
            remains.c_str(), [&](char const *end_key, node_pointer ptr) {
              // small_string sk(key);
              // sk += '.';
              // sk += end_key;
              UNUSED(end_key, ptr);
              _indexes.template emplace(std::make_pair(key, ptr));
              if (cb)
                cb(key, ptr);
            },
            a0, args...);
      } else {
        small_string k(key);
        auto it = _children.find(k);
        if (it == _children.end()) {
          _children.emplace(std::make_pair(k, std::make_shared<self_type>(self_type(std::forward<A>(a0), std::forward<Args>(args)...))));
          it = _children.find(k);
        } else
          it->second->emplace(a0, args...);

        // build index
        auto ptr = (it->second);
        _indexes.template emplace(std::make_pair(k, ptr));
        if (cb)
          cb(key, ptr);
        return;
      }
    }

    template<class A,
             std::enable_if_t<std::is_constructible<T, A>::value &&
                                  !std::is_same<std::decay_t<A>, self_type>::value,
                              int> = 0>
    void _set(char const *key, update_parent_index const &cb, A &&a) {
      std::unique_lock<std::shared_mutex> lock(mutex_);
      char const *part = std::strchr(key, '.');
      if (part) {
        small_string base(key);
        std::size_t pos = part - key;
        small_string k = base.substr(0, pos), remains = base.substr(pos + 1);
        auto it = _children.find(k);
        if (it == _children.end()) {
          _children.emplace(std::make_pair(k, std::make_shared<self_type>()));
          it = _children.find(k);
        }
        it->second->_set(
            remains.c_str(), [&](char const *end_key, node_pointer ptr) {
              // small_string sk(key);
              // sk += '.';
              // sk += end_key;
              UNUSED(end_key, ptr);
              _indexes.emplace(std::make_pair(key, ptr));
              if (cb)
                cb(key, ptr);
            },
            a);
      } else {
        small_string k(key);
        auto it = _children.find(k);
        if (it == _children.end()) {
          _children.emplace(std::make_pair(k, std::make_shared<self_type>(self_type(std::forward<A>(a)))));
          it = _children.find(k);
        } else
          it->second->emplace(a);

        // build index
        auto ptr = (it->second);
        _indexes.emplace(std::make_pair(k, ptr));
#if defined(CMDR_ENABLE_VERBOSE_LOG) && 0
        {
          std::ostringstream osdbg;
          osdbg << (*this);
          std::ostringstream osdbg1;
          osdbg1 << '?';
          cmdr_verbose_debug("     > [     ]      '%s' => '%s' (should be '%s')", key, osdbg.str().c_str(), osdbg1.str().c_str());
        }
#endif
        if (cb)
          cb(key, ptr);
        return;
      }
    }

  public:
    // T const &get(char const *key) const { return _get(key); }
    // T &get(char const *key) { return _get(key); }
    // [[nodiscard]] T const &get(std::string const &key) const { return _get(key); }
    // T &get(std::string const &key) { return _get(key); }

    [[nodiscard]] T const &get_raw(char const *key) const { return _get_raw(key); }
    T &get_raw(char const *key) { return _get_raw(key); }
    [[nodiscard]] T const &get_raw(std::string const &key) const { return _get_raw(key); }
    T &get_raw(std::string const &key) { return _get_raw(key); }

    [[nodiscard]] T const &get_raw_p(std::string const &prefix, std::string const &key) const { return _get_raw_p(prefix, key); }
    [[nodiscard]] T const &get_raw_p(char const *prefix, char const *key) const { return _get_raw_p(prefix, key); }
    T &get_raw_p(std::string const &prefix, std::string const &key) { return _get_raw_p(prefix, key); }
    T &get_raw_p(char const *prefix, char const *key) { return _get_raw_p(prefix, key); }

    bool has(char const *prefix, char const *key) const;

  private:
    // T &_get(std::string const &key);
    [[nodiscard]] T const &_get_raw(std::string const &key) const;
    T &_get_raw(std::string const &key);

    [[nodiscard]] T const &_get_raw_p(std::string const &prefix, std::string const &key) const;
    [[nodiscard]] T const &_get_raw_p(char const *prefix, char const *key) const;
    T &_get_raw_p(std::string const &prefix, std::string const &key);
    T &_get_raw_p(char const *prefix, char const *key);

  public:
    static T &null_element() {
      static T t{};
      return t;
    }

  private:
    void dump_tree(std::ostream &os,
                   tcolorize *c,
                   tcolorize::Colors256 dim_text_fg,
                   bool dim_text_dim,
                   int level = 0) const;
    // template<class K, class V>
    void dump_tree_f(std::ostream &os,
                     tcolorize *c,
                     tcolorize::Colors256 dim_text_fg,
                     bool dim_text_dim,
                     std::string const &key_prefix,
                     std::function<bool(std::pair<key_type, node_pointer> const &)> const &on_filter,
                     int level = 0) const;

    void dump_full_keys(std::ostream &os,
                        tcolorize *c,
                        tcolorize::Colors256 dim_text_fg,
                        bool dim_text_dim,
                        int level = 0) const;
    template<class K, class V, class Comp = std::less<K>>
    void dump_full_keys_f(std::ostream &os,
                          tcolorize *c,
                          tcolorize::Colors256 dim_text_fg,
                          bool dim_text_dim,
                          std::function<bool(std::pair<K, V> const &)> const &on_filter,
                          int level = 0) const;

    template<class K, class V>
    void walk_by_full_keys(std::function<void(std::pair<K, V> const &val)> const &cb) {
      walk_sorted(_indexes, cb);
    }

  private:
    template<class K, class V>
    class map_streamer {
      std::ostream &_os;
      tcolorize *c{};
      tcolorize::Colors256 fg{};
      bool dim{};
      std::function<bool(std::pair<K, V> const &val)> on_filter;

    public:
      map_streamer() = default;

      explicit map_streamer(std::ostream &os,
                            tcolorize *c,
                            tcolorize::Colors256 dim_text_fg,
                            bool dim_text_dim,
                            std::function<bool(std::pair<K, V> const &val)> on_filter_ = nullptr);

      // template<class K, class V>
      void operator()(std::pair<K, V> const &val);
    };

    template<class K, class V, class Comp = std::less<K>>
    void print_sorted(std::ostream &os,
                      tcolorize *c,
                      tcolorize::Colors256 dim_text_fg,
                      bool dim_text_dim,
                      std::unordered_map<K, V> const &um,
                      std::function<bool(std::pair<K, V> const &val)> const &on_filter = nullptr,
                      Comp comp                                                        = Comp()) const;

    template<class K, class V, class Comp = std::less<K>>
    void walk_sorted(std::unordered_map<K, V> const &um,
                     std::function<void(std::pair<K, V> const &val)> const &cb,
                     Comp comp = Comp()) const;

  }; // class nodeT<T, small_string>

  template<class T, class small_string>
  inline bool nodeT<T, small_string>::dump_with_type_name{true};


  template<class T, typename small_string>
  class treeT {
  public:
    using self_type    = treeT<T, small_string>;
    using key_type     = small_string;
    using node         = nodeT<T, small_string>;
    using node_pointer = std::shared_ptr<node>;
    using node_vec     = std::list<node_pointer>;
    using node_index   = std::unordered_map<small_string, node_pointer>;

  public:
    template<class A, typename... Args,
             std::enable_if_t<
#if __GNUC__ > 9
                 std::is_constructible<T, A, Args...>::value &&
#endif
                     !std::is_same<std::decay_t<A>, vars::variable>::value &&
                     !std::is_same<std::decay_t<A>, self_type>::value,
                 int> = 0>
    void set_raw(char const *key, A &&a0, Args &&...args) {
      _root.set(key, a0, args...);
    }
    template<class A, std::enable_if_t<
#if __GNUC__ > 9
                          std::is_constructible<T, A>::value &&
#endif
                              !std::is_same<std::decay_t<A>, vars::variable>::value &&
                              !std::is_same<std::decay_t<A>, self_type>::value,
                          int> = 0>
    void set_raw(char const *key, A &&a) {
      _root.set(key, a);
    }
    void set_raw(char const *key, vars::variable &&a) { _root.set(key, a.underlying_value()); }
    void set_raw(char const *key, vars::variable const &a) { _root.set(key, a.underlying_value()); }

  public:
    template<class A, typename... Args, std::enable_if_t<
#if __GNUC__ > 9
                                            std::is_constructible<T, A, Args...>::value &&
#endif
                                                !std::is_same<std::decay_t<A>, vars::variable>::value && !std::is_same<std::decay_t<A>, self_type>::value,
                                            int> = 0>
    void set_raw_p(char const *prefix, char const *key, A &&a0, Args &&...args) {
      std::ostringstream os;
      if (prefix && prefix[0] != 0) os << prefix << '.';
      os << key;
      _root.set(os.str().c_str(), a0, args...);
    }
    template<class A, std::enable_if_t<
#if __GNUC__ > 9
                          std::is_constructible<T, A>::value &&
#endif
                              !std::is_same<std::decay_t<A>, vars::variable>::value &&
                              !std::is_same<std::decay_t<A>, self_type>::value,
                          int> = 0>
    void set_raw_p(char const *prefix, char const *key, A &&a) {
      std::ostringstream os;
      if (prefix && prefix[0] != 0) os << prefix << '.';
      os << key;
      _root.set(os.str().c_str(), a);
    }
    void set_raw_p(char const *prefix, char const *key, vars::variable &&a) {
      std::ostringstream os;
      if (prefix && prefix[0] != 0) os << prefix << '.';
      os << key;
      _root.set(os.str().c_str(), a.underlying_value());
    }
    void set_raw_p(char const *prefix, char const *key, vars::variable const &a) {
      std::ostringstream os;
      if (prefix && prefix[0] != 0) os << prefix << '.';
      os << key;
      _root.set(os.str().c_str(), a.underlying_value());
    }

  public:
    // T const &get(char const *key) const { return _root.get(key); }
    // T &get(char const *key) { return _root.get(key); }
    // [[nodiscard]] T const &get(std::string const &key) const { return _root.get(key); }
    // [[nodiscard]] T &get(std::string const &key) { return _root.get(key); }

    T const &get_raw(char const *key) const { return _root.get_raw(key); }
    T &get_raw(char const *key) { return _root.get_raw(key); }
    [[nodiscard]] T const &get_raw(std::string const &key) const { return _root.get_raw(key); }
    T &get_raw(std::string const &key) { return _root.get_raw(key); }

    T const &get_raw_p(char const *prefix, char const *key) const { return _root.get_raw_p(prefix, key); }
    T &get_raw_p(char const *prefix, char const *key) { return _root.get_raw_p(prefix, key); }
    [[nodiscard]] T const &get_raw_p(std::string const &prefix, std::string const &key) const { return _root.get_raw_p(prefix, key); }
    T &get_raw_p(std::string const &prefix, std::string const &key) { return _root.get_raw_p(prefix, key); }

  public:
    bool has(char const *prefix, char const *key) const { return _root.has(prefix, key); }

    void dump_tree(tcolorize &c, tcolorize::Colors256 dim_text_fg, bool dim_text_dim,
                   std::ostream &os,
                   const_chars leading_title = nullptr,
                   node *start               = nullptr) const {
      if (leading_title)
        os << c.bold().s(leading_title);
      else
        os << c.bold().s("Dumping for var_t as Tree ...");
      os << '\n';
      (start ? start : &_root)->dump_tree(os, &c, dim_text_fg, dim_text_dim, 0);
    }
    // template<class K, class V, class Comp = std::less<K>>
    void dump_tree_f(tcolorize &c, tcolorize::Colors256 dim_text_fg, bool dim_text_dim,
                     std::ostream &os,
                     std::string const &key_prefix,
                     std::function<bool(std::pair<key_type, node_pointer> const &)> const &on_filter,
                     const_chars leading_title = nullptr,
                     node *start               = nullptr) const {
      if (leading_title)
        os << c.bold().s(leading_title);
      else
        os << c.bold().s("Dumping for var_t as Tree ...");
      os << '\n';
      auto ptr = (start ? start : &_root);
      ptr->dump_tree_f(os, &c, dim_text_fg, dim_text_dim, key_prefix, on_filter, 0);
    }
    void dump_full_keys(tcolorize &c, tcolorize::Colors256 dim_text_fg, bool dim_text_dim,
                        std::ostream &os,
                        const_chars leading_title = nullptr,
                        node *start               = nullptr) const {
      if (leading_title)
        os << c.bold().s(leading_title);
      else
        os << c.bold().s("Dumping for var_t ...");
      os << '\n';
      (start ? start : &_root)->dump_full_keys(os, &c, dim_text_fg, dim_text_dim, 0);
    }
    template<class K, class V, class Comp = std::less<K>>
    void dump_full_keys_f(tcolorize &c, tcolorize::Colors256 dim_text_fg, bool dim_text_dim,
                          std::ostream &os,
                          std::function<bool(std::pair<K, V> const &)> const &on_filter,
                          const_chars leading_title = nullptr,
                          node *start               = nullptr) const {
      if (leading_title)
        os << c.bold().s(leading_title);
      else
        os << c.bold().s("Dumping for var_t ...");
      os << '\n';
      (start ? start : &_root)->dump_full_keys_f(os, &c, dim_text_fg, dim_text_dim, on_filter, 0);
    }

    void walk_by_full_keys(std::function<void(std::pair<small_string, node_pointer> const &val)> const &cb, node *start = nullptr) {
      (start ? start : &_root)->walk_by_full_keys(cb);
    }

    void reset() {
      _root.reset();
    }

  private:
    node _root;
  }; // class treeT<T, small_string>


  class store : public treeT<variable, std::string> {
  public:
    store()            = default;
    ~store()           = default;

    using parent_type  = treeT<variable, std::string>;
    // using self_type = treeT<variable, std::string>;
    using key_type     = std::string;
    using node         = nodeT<variable, std::string>;
    using node_pointer = std::shared_ptr<node>;
    using node_vec     = std::list<node_pointer>;
    using node_index   = std::unordered_map<std::string, node_pointer>;

    static tcolorize _c;
    static tcolorize::Colors256 _dim_text_fg;
    static bool _dim_text_dim;
    static bool _long_title_underline;

    static std::string dark_text(std::string const &s) {
      std::ostringstream os;
      os << _c.fg(_dim_text_fg).s(s);
      return os.str();
    }
    static std::string dark_text(std::string &&s) {
      std::ostringstream os;
      os << _c.fg(_dim_text_fg).s(s);
      return os.str();
    }

    void set_dump_with_type_name(bool b) { node::dump_with_type_name = b; }
    bool get_dump_with_type_name() const { return node::dump_with_type_name; }


    friend std::ostream &operator<<(std::ostream &os, store const &o) {
      o.dump_tree(os);
      return os;
    }


    void dump_tree_f(std::ostream &os                                                                = std::cout,
                     std::function<bool(std::pair<key_type, node_pointer> const &)> const &on_filter = nullptr,
                     const_chars leading_title                                                       = nullptr,
                     node *start                                                                     = nullptr) const {
      parent_type::dump_tree_f(_c, _dim_text_fg, _dim_text_dim,
                               os, "", on_filter, leading_title, start);
    }
    void dump_full_keys_f(std::ostream &os                                                                = std::cout,
                          std::function<bool(std::pair<key_type, node_pointer> const &)> const &on_filter = nullptr,
                          const_chars leading_title                                                       = nullptr,
                          node *start                                                                     = nullptr) const {
      parent_type::dump_full_keys_f(_c, _dim_text_fg, _dim_text_dim,
                                    os, on_filter, leading_title, start);
    }

    void dump_tree(std::ostream &os          = std::cout,
                   const_chars leading_title = nullptr,
                   vars::store::node *start  = nullptr) const {
      parent_type::dump_tree(_c, _dim_text_fg, _dim_text_dim,
                             os, leading_title, start);
    }
    void dump_full_keys(std::ostream &os          = std::cout,
                        const_chars leading_title = nullptr,
                        vars::store::node *start  = nullptr) const {
      parent_type::dump_full_keys(_c, _dim_text_fg, _dim_text_dim,
                                  os, leading_title, start);
    }

  }; // class store

  inline tcolorize store::_c{tcolorize::create()};
  inline tcolorize::Colors256 store::_dim_text_fg{tcolorize::Colors256::Grey50};
  inline bool store::_dim_text_dim{false};
  inline bool store::_long_title_underline{true};


} // namespace cmdr::vars

#include "cmdr_var_t_inl.h"

#endif // CMDR_CXX11_CMDR_VAR_T_HH
