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


#include "cmdr_chrono.hh"
#include "cmdr_defs.hh"
#include "cmdr_string.hh"
#include "cmdr_terminal.hh"
#include "cmdr_types.hh"
#include "cmdr_types_check.hh"


namespace cmdr::vars {


    namespace details {
        template<class T, class F>
        inline std::pair<const std::type_index, std::function<void(std::ostream &os, std::any const &)>>
        to_any_visitor(F const &f) {
            return {
                    std::type_index(typeid(T)),
                    [g = f](std::ostream &os, std::any const &a) {
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
                        if constexpr (std::is_void_v<T>) {
                            g(is);
                        } else if constexpr (std::is_same_v<typename lambda_func_type<decltype(g)>::type, void(std::istream &, std::any &) const>) {
                            g(is, a);
                        } else {
                            T t;
                            g(is, t);
                            a.template emplace<T>(t);
                        }
                    }};
        }
    } // namespace details


    class variable {
    public:
        typedef variable self_type;
        using target_type = std::any;

        variable() = default;
        template<class A, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<target_type, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, variable>::value &&
                                 !std::is_same<std::decay_t<A>, std::any>::value &&
                                 !std::is_same<std::decay_t<A>, self_type>::value,
                         int> = 0>
        explicit variable(A &&a0, Args &&...args)
            : _value(std::forward<A>(a0), std::forward<Args>(args)...) {}
        template<class A,
                 std::enable_if_t<
                         std::is_constructible<target_type, A>::value &&
                                 !std::is_same<std::decay_t<A>, variable>::value &&
                                 !std::is_same<std::decay_t<A>, std::any>::value &&
                                 !std::is_same<std::decay_t<A>, self_type>::value,
                         int> = 0>
        explicit variable(A &&a)
            : _value(std::forward<A>(a)) {}
        // explicit variable(variable &&a)
        //     : _value(std::move(a.value_any())) {}
        explicit variable(std::any &&a)
            : _value(std::move(a)) {}
        // explicit variable(variable const &a)
        //     : _value(std::move(a.value_any())) {}
        explicit variable(std::any const &a)
            : _value(std::move(a)) {}
        virtual ~variable() = default;

        template<typename T>
        variable &operator=(T const &o) {
            _value = o;
            return (*this);
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

        [[nodiscard]] target_type const &value_any() const { return _value; }
        target_type &value_any() { return _value; }

        [[nodiscard]] std::string as_string() const {
            std::stringstream os;
            os << (*this);
            return os.str();
        }

        template<class T>
        T cast_as() const {
            return std::any_cast<T>(_value);
        }

        [[nodiscard]] bool has_value() const { return _value.has_value(); }
        [[nodiscard]] bool empty() const { return !_value.has_value(); }
        void reset() { _value.reset(); }
        [[nodiscard]] const std::type_info &type() const noexcept { return _value.type(); }

    public:
        static self_type parse(std::string &s);

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
            while ((ok = string::read_until(is, s, ","))) {
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
            char ch;
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
                } else {
                    x = false;
                    return true;
                }
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
            static I _registry{
                    details::from_any_visitor<void>([](std::istream &is) { parse_void(is); }),
                    details::from_any_visitor<bool>([](std::istream &is, bool &x) { parse_bool(is, x); }),
                    details::from_any_visitor<int>([](std::istream &is, int &x) { is >> x; }),
                    details::from_any_visitor<int8_t>([](std::istream &is, int8_t &x) { is >> x; }),
                    details::from_any_visitor<int16_t>([](std::istream &is, int16_t &x) { is >> x; }),
                    details::from_any_visitor<int32_t>([](std::istream &is, int32_t &x) { is >> x; }),
                    details::from_any_visitor<int64_t>([](std::istream &is, int64_t &x) { is >> x; }),
                    details::from_any_visitor<unsigned>([](std::istream &is, unsigned &x) { is >> x; }),
                    details::from_any_visitor<uint8_t>([](std::istream &is, uint8_t &x) { is >> x; }),
                    details::from_any_visitor<uint16_t>([](std::istream &is, uint16_t &x) { is >> x; }),
                    details::from_any_visitor<uint32_t>([](std::istream &is, uint32_t &x) { is >> x; }),
                    details::from_any_visitor<uint64_t>([](std::istream &is, uint64_t &x) { is >> x; }),
                    details::from_any_visitor<long>([](std::istream &is, long &x) { is >> x; }),
                    details::from_any_visitor<unsigned long>([](std::istream &is, unsigned long &x) { is >> x; }),
                    details::from_any_visitor<long long>([](std::istream &is, long long &x) { is >> x; }),
                    details::from_any_visitor<unsigned long long>([](std::istream &is, unsigned long long &x) { is >> x; }),
                    details::from_any_visitor<float>([](std::istream &is, float &x) { is >> x; }),
                    details::from_any_visitor<double>([](std::istream &is, double &x) { is >> x; }),
                    details::from_any_visitor<long double>([](std::istream &is, long double &x) { is >> x; }),
                    details::from_any_visitor<char const *>([](std::istream &is, std::any &a) { std::string s; string::strip_quotes(is, s); a=s; }),
                    details::from_any_visitor<std::string>([](std::istream &is, std::string &s) { string::strip_quotes(is, s); }),
                    details::from_any_visitor<variable>([](std::istream &is, variable &x) { is >> x; }),

                    details::from_any_visitor<std::vector<char const *>>([](std::istream &is, std::any &a) { std::vector<std::string> vec; parse_array(is, vec);a=vec; }),
                    details::from_any_visitor<std::vector<std::string>>([](std::istream &is, std::vector<std::string> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<bool>>([](std::istream &is, std::vector<bool> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<int>>([](std::istream &is, std::vector<int> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<int8_t>>([](std::istream &is, std::vector<int8_t> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<int16_t>>([](std::istream &is, std::vector<int16_t> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<int32_t>>([](std::istream &is, std::vector<int32_t> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<int64_t>>([](std::istream &is, std::vector<int64_t> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<unsigned>>([](std::istream &is, std::vector<unsigned> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<uint8_t>>([](std::istream &is, std::vector<uint8_t> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<uint16_t>>([](std::istream &is, std::vector<uint16_t> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<uint32_t>>([](std::istream &is, std::vector<uint32_t> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<uint64_t>>([](std::istream &is, std::vector<uint64_t> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<long>>([](std::istream &is, std::vector<long> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<unsigned long>>([](std::istream &is, std::vector<unsigned long> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<long long>>([](std::istream &is, std::vector<long long> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<unsigned long long>>([](std::istream &is, std::vector<unsigned long long> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<float>>([](std::istream &is, std::vector<float> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<double>>([](std::istream &is, std::vector<double> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<long double>>([](std::istream &is, std::vector<long double> &a) { parse_array(is, a); }),
                    details::from_any_visitor<std::vector<variable>>([](std::istream &is, std::vector<variable> &a) { parse_array(is, a); }),

                    details::from_any_visitor<std::complex<float>>([](std::istream &is, std::complex<float> &a) { parse_complex(is, a); }),
                    details::from_any_visitor<std::complex<double>>([](std::istream &is, std::complex<double> &a) { parse_complex(is, a); }),
                    details::from_any_visitor<std::complex<long double>>([](std::istream &is, std::complex<long double> &a) { parse_complex(is, a); }),

                    details::from_any_visitor<std::chrono::nanoseconds>([](std::istream &is, std::chrono::nanoseconds &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::microseconds>([](std::istream &is, std::chrono::microseconds &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::milliseconds>([](std::istream &is, std::chrono::milliseconds &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::seconds>([](std::istream &is, std::chrono::seconds &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::minutes>([](std::istream &is, std::chrono::minutes &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::hours>([](std::istream &is, std::chrono::hours &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::duration<long double, std::ratio<1>>>([](std::istream &is, std::chrono::duration<long double, std::ratio<1>> &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::duration<long double, std::ratio<60>>>([](std::istream &is, std::chrono::duration<long double, std::ratio<60>> &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::duration<double, std::ratio<60>>>([](std::istream &is, std::chrono::duration<double, std::ratio<60>> &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::duration<float, std::ratio<60>>>([](std::istream &is, std::chrono::duration<float, std::ratio<60>> &x) { cmdr::chrono::parse_duration(is, x); }),
                    details::from_any_visitor<std::chrono::duration<float, std::ratio<1>>>([](std::istream &is, std::chrono::duration<float, std::ratio<1>> &x) { cmdr::chrono::parse_duration(is, x); }),
            };
            return _registry;
        }
        typedef std::unordered_map<std::type_index, std::function<void(std::ostream &os, std::any const &)>> R;
        static R &any_visitors() {
            static R _registry{
                    details::to_any_visitor<void>([](std::ostream &os) { os << "{}"; }),
                    details::to_any_visitor<bool>([](std::ostream &os, bool x) { os << std::boolalpha << x; }),
                    details::to_any_visitor<int>([](std::ostream &os, int x) { os << x; }),
                    details::to_any_visitor<int8_t>([](std::ostream &os, int8_t x) { os << x; }),
                    details::to_any_visitor<int16_t>([](std::ostream &os, int16_t x) { os << x; }),
                    details::to_any_visitor<int32_t>([](std::ostream &os, int32_t x) { os << x; }),
                    details::to_any_visitor<int64_t>([](std::ostream &os, int64_t x) { os << x; }),
                    details::to_any_visitor<unsigned>([](std::ostream &os, unsigned x) { os << x; }),
                    details::to_any_visitor<uint8_t>([](std::ostream &os, uint8_t x) { os << x; }),
                    details::to_any_visitor<uint16_t>([](std::ostream &os, uint16_t x) { os << x; }),
                    details::to_any_visitor<uint32_t>([](std::ostream &os, uint32_t x) { os << x; }),
                    details::to_any_visitor<uint64_t>([](std::ostream &os, uint64_t x) { os << x; }),
                    details::to_any_visitor<long>([](std::ostream &os, long x) { os << x; }),
                    details::to_any_visitor<unsigned long>([](std::ostream &os, unsigned long x) { os << x; }),
                    details::to_any_visitor<long long>([](std::ostream &os, long long x) { os << x; }),
                    details::to_any_visitor<unsigned long long>([](std::ostream &os, unsigned long long x) { os << x; }),
                    details::to_any_visitor<float>([](std::ostream &os, float x) { os << x; }),
                    details::to_any_visitor<double>([](std::ostream &os, double x) { os << x; }),
                    details::to_any_visitor<long double>([](std::ostream &os, long double x) { os << x; }),
                    details::to_any_visitor<char const *>([](std::ostream &os, char const *s) { os << std::quoted(s); }),
                    details::to_any_visitor<std::string>([](std::ostream &os, std::string const &s) { os << std::quoted(s); }),
                    details::to_any_visitor<variable>([](std::ostream &os, variable x) { os << x; }),

                    details::to_any_visitor<std::vector<char const *>>([](std::ostream &os, std::vector<char const *> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<std::string>>([](std::ostream &os, std::vector<std::string> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<bool>>([](std::ostream &os, std::vector<bool> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<int>>([](std::ostream &os, std::vector<int> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<int8_t>>([](std::ostream &os, std::vector<int8_t> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<int16_t>>([](std::ostream &os, std::vector<int16_t> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<int32_t>>([](std::ostream &os, std::vector<int32_t> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<int64_t>>([](std::ostream &os, std::vector<int64_t> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<unsigned>>([](std::ostream &os, std::vector<unsigned> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<uint8_t>>([](std::ostream &os, std::vector<uint8_t> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<uint16_t>>([](std::ostream &os, std::vector<uint16_t> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<uint32_t>>([](std::ostream &os, std::vector<uint32_t> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<uint64_t>>([](std::ostream &os, std::vector<uint64_t> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<long>>([](std::ostream &os, std::vector<long> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<unsigned long>>([](std::ostream &os, std::vector<unsigned long> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<long long>>([](std::ostream &os, std::vector<long long> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<unsigned long long>>([](std::ostream &os, std::vector<unsigned long long> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<float>>([](std::ostream &os, std::vector<float> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<double>>([](std::ostream &os, std::vector<double> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<long double>>([](std::ostream &os, std::vector<long double> const &a) { format_array(os, a); }),
                    details::to_any_visitor<std::vector<variable>>([](std::ostream &os, std::vector<variable> const &a) { format_array(os, a); }),

                    details::to_any_visitor<std::complex<float>>([](std::ostream &os, std::complex<float> const &a) { format_complex(os, a); }),
                    details::to_any_visitor<std::complex<double>>([](std::ostream &os, std::complex<double> const &a) { format_complex(os, a); }),
                    details::to_any_visitor<std::complex<long double>>([](std::ostream &os, std::complex<long double> const &a) { format_complex(os, a); }),

                    details::to_any_visitor<std::chrono::nanoseconds>([](std::ostream &os, const std::chrono::nanoseconds &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::microseconds>([](std::ostream &os, const std::chrono::microseconds &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::milliseconds>([](std::ostream &os, const std::chrono::milliseconds &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::seconds>([](std::ostream &os, const std::chrono::seconds &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::minutes>([](std::ostream &os, const std::chrono::minutes &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::hours>([](std::ostream &os, const std::chrono::hours &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::duration<long double, std::ratio<1>>>([](std::ostream &os, const std::chrono::duration<long double, std::ratio<1>> &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::duration<long double, std::ratio<60>>>([](std::ostream &os, const std::chrono::duration<long double, std::ratio<60>> &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::duration<double, std::ratio<60>>>([](std::ostream &os, const std::chrono::duration<double, std::ratio<60>> &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::duration<float, std::ratio<60>>>([](std::ostream &os, const std::chrono::duration<float, std::ratio<60>> &x) { cmdr::chrono::format_duration(os, x); }),
                    details::to_any_visitor<std::chrono::duration<float, std::ratio<1>>>([](std::ostream &os, const std::chrono::duration<float, std::ratio<1>> &x) { cmdr::chrono::format_duration(os, x); }),
                    // ... add more handlers for your types ...
            };
            return _registry;
        }

        inline bool process(std::istream &is, std::any &a) const {
            auto &reg = any_parsers();
            if (const auto it = reg.find(std::type_index(a.type()));
                it != reg.cend()) {
                it->second(is, a);
                return true;
            } else {
#if defined(_DEBUG)
                std::cout << "Unregistered type for parsing " << std::quoted(a.type().name()) << "\ufe0e" << '\n';
                std::cerr << "Unregistered type for parsing " << std::quoted(a.type().name()) << "\ufe0e" << '\n';
#endif
                return false;
            }
        }
        inline bool process(std::ostream &os, std::any const &a) const {
            auto &reg = any_visitors();
            if (const auto it = reg.find(std::type_index(a.type()));
                it != reg.cend()) {
                it->second(os, a);
                return true;
            } else {
#if defined(_DEBUG)
                std::cout << "Unregistered type for visiting " << std::quoted(a.type().name()) << "\ufe0e" << '\n';
                std::cerr << "Unregistered type for visiting " << std::quoted(a.type().name()) << "\ufe0e" << '\n';
#endif
                return false;
            }
        }

        template<class T, class F>
        inline void register_visitor(F const &f) {
            std::cout << "Register visitor for type "
                      << std::quoted(typeid(T).name()) << '\n';
            any_visitors().insert(details::to_any_visitor<T>(f));
        }

        friend std::ostream &operator<<(std::ostream &os, variable const &o) {
            o.process(os, o._value);
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
         *     std::stringstream ss("true");
         *     cmdr::vars::variable v{false}; // or v{} to initialize a bool value implicitly.
         *     ss >> v; // so that we can convert the input stream with that type.
         *     std::cout << std::boolalpha << v; // print v will get this output: true.
         *     
         */
        friend std::istream &operator>>(std::istream &is, variable &o) {
            o.process(is, o._value);
            return is;
        }

    private:
        target_type _value;
    }; // class variable

    // variable:
    //


    template<class T, typename small_string = std::string>
    class treeT;

    template<class T, class small_string = std::string>
    class nodeT;


    template<class T, class small_string>
    class nodeT : public T {
    public:
        friend class treeT<T, small_string>;
        typedef nodeT<T, small_string> self_type;
        using node_vec = std::list<self_type>;
        using node_pointer = self_type *;
        using node_map = std::unordered_map<small_string, self_type>;
        using node_idx = std::unordered_map<small_string, node_pointer>;

        nodeT() = default;
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

    private:
        node_map _children{};
        node_idx _indexes{};
        node_pointer _parent{};

    public:
        template<class A, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<T, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, self_type>::value,
                         int> = 0>
        void set(char const *key, A &&a0, Args &&...args) {
            this->_set(
                    key, [&](char const *, node_pointer) {}, a0, args...);
        }
        template<class A,
                 std::enable_if_t<std::is_constructible<T, A>::value &&
                                          !std::is_same<std::decay_t<A>, self_type>::value,
                                  int> = 0>
        void set(char const *key, A &&a) {
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
                    _children.emplace(std::make_pair(k, self_type{}));
                    it = _children.find(k);
                }
                it->second._set(
                        remains.c_str(), [&](char const *end_key, node_pointer ptr) {
                            // small_string sk(key);
                            // sk += '.';
                            // sk += end_key;
                            unused(end_key);
                            _indexes.template emplace(std::make_pair(key, ptr));
                            if (cb)
                                cb(key, ptr);
                        },
                        a0, args...);
            } else {
                small_string k(key);
                auto it = _children.find(k);
                if (it == _children.end()) {
                    _children.emplace(std::make_pair(k, self_type(std::forward<A>(a0), std::forward<Args>(args)...)));
                    it = _children.find(k);
                } else
                    it->second.emplace(a0, args...);

                // build index
                auto ptr = &(it->second);
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
            char const *part = std::strchr(key, '.');
            if (part) {
                small_string base(key);
                std::size_t pos = part - key;
                small_string k = base.substr(0, pos), remains = base.substr(pos + 1);
                auto it = _children.find(k);
                if (it == _children.end()) {
                    _children.emplace(std::make_pair(k, self_type{}));
                    it = _children.find(k);
                }
                it->second._set(
                        remains.c_str(), [&](char const *end_key, node_pointer ptr) {
                            // small_string sk(key);
                            // sk += '.';
                            // sk += end_key;
                            unused(end_key);
                            _indexes.template emplace(std::make_pair(key, ptr));
                            if (cb)
                                cb(key, ptr);
                        },
                        a);
            } else {
                small_string k(key);
                auto it = _children.find(k);
                if (it == _children.end()) {
                    _children.emplace(std::make_pair(k, self_type(std::forward<A>(a))));
                    it = _children.find(k);
                } else
                    it->second.emplace(a);

                // build index
                auto ptr = &(it->second);
                _indexes.template emplace(std::make_pair(k, ptr));
                if (cb)
                    cb(key, ptr);
                return;
            }
        }

    public:
        T const &get(char const *key) const { return _get(key); }
        T &get(char const *key) { return _get(key); }
        [[nodiscard]] T const &get(std::string const &key) const { return _get(key); }
        T &get(std::string const &key) { return _get(key); }

        T const &get_raw(char const *key) const { return _get_raw(key); }
        T &get_raw(char const *key) { return _get_raw(key); }
        [[nodiscard]] T const &get_raw(std::string const &key) const { return _get_raw(key); }
        T &get_raw(std::string const &key) { return _get_raw(key); }

    private:
        T &_get(std::string const &key);
        T &_get_raw(std::string const &key);

    public:
        static T &null_element() {
            static T t{};
            return t;
        }

    private:
        void dump_tree(std::ostream &os, cmdr::terminal::colors::colorize *c, int level) const;
        void dump_full_keys(std::ostream &os, cmdr::terminal::colors::colorize *c, int level = 0) const;

        template<class K, class V>
        void walk_by_full_keys(std::function<void(std::pair<K, V> const &val)> const &cb) {
            walk_sorted(_indexes, cb);
        }

    private:
        struct map_streamer {
            std::ostream &_os;
            cmdr::terminal::colors::colorize *c;
            cmdr::terminal::colors::colorize::Colors256 fg;
            bool dim;

            explicit map_streamer(std::ostream &os, cmdr::terminal::colors::colorize *c);

            template<class K, class V>
            void operator()(std::pair<K, V> const &val) {
                // .first is your key, .second is your value
                _os << " - " << val.first << " : ";
                std::stringstream ss;
                ss << (*val.second);
                if (c)
                    _os << c->fg(fg).dim(dim).s(ss.str());
                else
                    _os << ss.str();
                _os << "\n";
            }
        };

        template<class K, class V, class Comp = std::less<K>>
        inline void print_sorted(std::ostream &os, cmdr::terminal::colors::colorize *c, std::unordered_map<K, V> const &um, Comp pred = Comp()) const {
            std::map<K, V> m(um.begin(), um.end(), pred);
            std::for_each(m.begin(), m.end(), map_streamer(os, c));
        }

        template<class K, class V, class Comp = std::less<K>>
        inline void walk_sorted(std::unordered_map<K, V> const &um,
                                std::function<void(std::pair<K, V> const &val)> const &cb,
                                Comp pred = Comp()) const {
            std::map<K, V> m(um.begin(), um.end(), pred);
            std::for_each(m.begin(), m.end(), cb);
        }

    }; // class nodeT<T>

    template<class T, typename small_string>
    class treeT {
    public:
        using self_type = treeT<T, small_string>;
        using node = nodeT<T, small_string>;
        using node_pointer = node *;
        using node_vec = std::list<node>;
        using node_index = std::unordered_map<small_string, node_pointer>;

    public:
        template<class A, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<T, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, vars::variable>::value &&
                                 !std::is_same<std::decay_t<A>, self_type>::value,
                         int> = 0>
        void set(char const *key, A &&a0, Args &&...args) {
            _root.template set(key, a0, args...);
        }
        template<class A,
                 std::enable_if_t<std::is_constructible<T, A>::value &&
                                          !std::is_same<std::decay_t<A>, vars::variable>::value &&
                                          !std::is_same<std::decay_t<A>, self_type>::value,
                                  int> = 0>
        void set(char const *key, A &&a) {
            _root.template set(key, a);
        }
        void set(char const *key, vars::variable &&a) { _root.set(key, a.value_any()); }
        void set(char const *key, vars::variable const &a) { _root.set(key, a.value_any()); }

        T const &get(char const *key) const { return _root.get(key); }
        T &get(char const *key) { return _root.get(key); }
        [[nodiscard]] T const &get(std::string const &key) const { return _root.get(key); }
        [[nodiscard]] T &get(std::string const &key) { return _root.get(key); }

        T const &get_raw(char const *key) const { return _root.get_raw(key); }
        T &get_raw(char const *key) { return _root.get_raw(key); }
        [[nodiscard]] T const &get_raw(std::string const &key) const { return _root.get_raw(key); }
        T &get_raw(std::string const &key) { return _root.get_raw(key); }

    public:
        void dump_tree(std::ostream &os, const_chars leading_title = nullptr, node *start = nullptr) const {
            auto c = cmdr::terminal::colors::colorize::create();
            if (leading_title)
                os << leading_title;
            else
                os << "Dumping for var_t as Tree ...";
            os << std::endl;
            (start ? start : &_root)->dump_tree(os, &c, 0);
        }
        void dump_full_keys(std::ostream &os, const_chars leading_title = nullptr, node *start = nullptr) const {
            auto c = cmdr::terminal::colors::colorize::create();
            if (leading_title)
                os << leading_title;
            else
                os << "Dumping for var_t ...";
            os << std::endl;
            (start ? start : &_root)->dump_full_keys(os, &c, 0);
        }

        void walk_by_full_keys(std::function<void(std::pair<small_string, node_pointer> const &val)> const &cb, node *start = nullptr) {
            (start ? start : &_root)->walk_by_full_keys(cb);
        }

    private:
        node _root;
    }; // class treeT<T>


#if 0
    struct streamable_any : std::any {
        void (*streamer)(std::ostream &, streamable_any const &); //= nullptr;
        friend std::ostream &operator<<(std::istream &is, streamable_any const &a) {
            a.streamer(os, a);
            return os;
        }

        [[nodiscard]] std::string as_string() const {
            std::stringstream os;
            if (streamer)
                streamer(os, *this);
            return os.str();
        }

        template<class T>
        const T &get() const {
            return std::any_cast<T>(*this);
        }

        template<class T,
                 typename std::enable_if<
                         !is_duration<std::decay_t<T>>::value &&
                         !is_stl_container<std::decay_t<T>>::value &&
                         !std::is_same<std::decay_t<T>, streamable_any>{}>::type * = nullptr>
        explicit streamable_any(T &&t)
            : std::any(std::forward<T>(t))
            , streamer([](std::istream &is, streamable_any const &self) {
                static_assert(!is_duration<std::decay_t<T>>::value);
                static_assert(!is_stl_container<T>::value);
                static_assert(!std::is_same<std::decay_t<T>, streamable_any>{});
                if constexpr (!std::is_void_v<T>) {
                    //if constexpr (is_duration<std::decay_t<T>>::value)
                    //    os << "norm - ";
                    os << std::any_cast<std::decay_t<T>>(self);
                    //auto const &vv = std::any_cast<std::decay_t<T>>(self);
                    //os << typeid(vv).name() << " (T=" << typeid(T).name() << ")";

                    // (void)(os);(void)(self);
                    //_format(os, std::any_cast<std::decay_t<T>>(self));
                    // if constexpr (is_duration<T>::value)
                    //     format_duration(os, std::any_cast<std::decay_t<T>>(self));
                    // else
                    //     os << std::any_cast<std::decay_t<T>>(self);
                }
            }) {}

        // template<class T, std::enable_if_t<!is_duration<T>::value, int> = 0>
        // static void _format(std::istream &is, T const &v) {
        //     os << v;
        // }
        // template<class T>
        // static void _format<T,
        //                     std::enable_if_t<is_duration<T>::value, int>>(std::istream &is, T const &v) {
        //     format_duration(os, v);
        // }

        template<class U>
        explicit streamable_any(U &&t, typename std::enable_if<is_duration<std::decay_t<U>>::value>::type * = nullptr)
            : std::any(std::forward<U>(t))
            , streamer([](std::istream &is, streamable_any const &self) {
                static_assert(is_duration<std::decay_t<U>>::value);
                // os << "[**duration**] ";
                chrono::format_duration(os, std::any_cast<std::decay_t<U>>(self));
            }) {}

        template<class V>
        explicit streamable_any(V &&t, typename std::enable_if<is_stl_container<std::decay_t<V>>::value>::type * = nullptr)
            : std::any(std::forward<V>(t))
            , streamer([](std::istream &is, streamable_any const &self) {
                os << '[';
                for (auto const v : std::any_cast<std::decay_t<V>>(self)) {
                    os << v << ',';
                }
                os << ']';
            }) {}

        // specialize for bool streaming output
        explicit streamable_any(bool t)
            : std::any(std::forward<bool>(t))
            , streamer([](std::istream &is, streamable_any const &self) {
                os << (std::any_cast<std::decay_t<bool>>(self) ? "true" : "false");
            }) {}

        streamable_any()
            : streamer([](std::ostream &, streamable_any const &) {}) {}
        ~streamable_any() {}

        template<class T>
        streamable_any &operator=(T const &v) {
            std::any::operator=(v);
            return (*this);
        }
    };
#endif

#if 0
    /**
     * @brief a node at a hierarchical data structure
     * @tparam holderT the real value type stored in this node
     *
     * A node holds a key-part and the underlying value.
     * We can access a node by its full dotted-separated path, for
     * example, 'app.server' and 'app.server.tls' are two nodes.
     */
    template<typename holderT = streamable_any>
    class var_t {
    public:
        typedef var_t self_type;
        typedef std::list<self_type> var_t_array;
        typedef std::unordered_map<std::string, var_t> children_container;
        typedef std::unordered_map<std::string, var_t *> child_pointers;

        const int NODE_DEFAULT_VALUE{0};
        const bool LOGGING_ENABLED{};

    public:
        var_t() = default;
        ~var_t() = default;
        var_t(var_t const &) = default;
        var_t(var_t &&) noexcept = default;

        template<typename A = holderT, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<holderT, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, var_t>::value,
                         int> = 0>
        explicit var_t(A &&a0, Args &&...args)
            : _value(std::forward<A>(a0), std::forward<Args>(args)...) {}

        explicit var_t(holderT &&v)
            : _value(std::move(v)) {}

        bool operator==(const var_t &r) {
            return _value == r._value;
        }

        template<class T>
        var_t &operator=(T const &v) {
            _value.operator=(v);
            return (*this);
        }

    public:
        var_t &get(const_chars key) { return _get(key); }
        var_t &get(const std::string &key) { return _get(key); }
        const var_t &get(const_chars key) const { return _get(key); }
        const var_t &get(const std::string &key) const { return _get(key); }

        [[nodiscard]] bool has(const std::string &key) const { return _full_key_map.find(key) != _full_key_map.end(); }

    private:
        var_t &_get(const std::string &key) {
            auto it = _full_key_map.find(key);
            if (it == _full_key_map.end())
                throw std::invalid_argument("key is not exists");
            return (*(it->second));
        }
        var_t const &_get(const std::string &key) const {
            auto const it = _full_key_map.find(key);
            if (it == _full_key_map.end())
                return null_elem();
            return (*(it->second));
        }

    public:
        // void set(const_chars key, const holderT &val) {
        //     std::string ks(key);
        //     set(ks, val);
        // }

        void set(const std::string &key, holderT &&val, self_type *parent) { _put_no_parent(key, val, parent); }
        void set(const_chars key, holderT &&val, self_type *parent) { _put_no_parent(key, val, parent); }
        void set(const_chars key, const holderT &val, self_type *parent) { _put_no_parent(key, val, parent); }

    private:
        void _put_no_parent(const std::string &key, const holderT &val, self_type *parent) {
            _put(key, val, parent, [this, key](const std::string &prefix, self_type *ptr) {
                // auto sk = key;
                // sk += '.';
                // sk += prefix;
                // if (auto it = _full_key_map.find(prefix); it == _full_key_map.end()) {
                //     _full_key_map.template emplace(sk, ptr);
                // }
                (void) (prefix);
                (void) (ptr);
#if defined(TMP_TMP)
                if (LOGGING_ENABLED)
                    std::cout << "      > this: '" << (*this) << "', key: " << key << ", matching key: " << prefix << std::endl;
#endif
                (void) (this);
                (void) (key);
            });
        }

        void _put(const std::string &key, const holderT &val, self_type *parent,
                  std::function<void(const std::string &, self_type *)> const &on_picked) {
            unused(parent);
            std::string prefix1, part1, remains;
            std::string::size_type pos{0}, old{0}, length = key.length();
            var_t *ptr;
            if (pos < length) {
                pos = key.find('.', old);
                if (pos != std::string::npos) {
                    part1 = key.substr(old, pos);
                    old = pos + 1;
                    remains = key.substr(old);
                } else {
                    part1 = key;
                    pos = length;
                    remains.clear();
                }

                // TODO optimize part1 with a small string pointer PTR, using PTR as the key of the hash_map (_children) and _key_part

                typename children_container::iterator itc = _children.find(part1);
                if (itc == _children.end()) {
                    if (remains.empty())
                        _children.template emplace(part1, val);
                    else
                        _children.template emplace(part1, NODE_DEFAULT_VALUE == 0 ? holderT() : holderT(NODE_DEFAULT_VALUE));
                    itc = _children.find(part1);
                }

                ptr = &((*itc).second);
                ptr->_parent = parent;

#if defined(TMP_TMP)
                if (LOGGING_ENABLED)
                    std::cout << "   -> putting '" << (*ptr) << "' into \"" << key << "\".\n";
#endif

                prefix1 = key.substr(0, pos);
                if (auto it = _full_key_map.find(prefix1); it == _full_key_map.end()) {
                    _full_key_map.template emplace(prefix1, ptr);
#if defined(TMP_TMP)
                    if (LOGGING_ENABLED)
                        std::cout << "      _full_key_map['" << prefix1 << "'] = '" << (*ptr) << "'\n";
#endif
                }
                if (on_picked)
                    on_picked(prefix1, ptr);

                // auto p = this;
                // while (p._parent != nullptr) {
                //     p = p._parent;
                //     // add this ptr to the full_key_map of parents
                //     prefix1 = p
                // }

                if (!remains.empty()) {
                    ptr->_put(remains, val, ptr, [this, on_picked, prefix1](const std::string &prefix, self_type *ptr) {
                        auto sk = prefix1;
                        sk += '.';
                        sk += prefix;
                        //if (this->_parent != nullptr) {
                        if (auto it = _full_key_map.find(sk); it == _full_key_map.end()) {
                            _full_key_map.template emplace(sk, ptr);
#if defined(TMP_TMP)
                            if (LOGGING_ENABLED)
                                std::cout << "      ^ _full_key_map['" << sk << "'] = '" << (*ptr) << "'\n";
                            // std::cout << "      putting '" << (*ptr) << "' into \"" << sk << "\".\n";
#endif
                        }
                        // }

                        if (on_picked)
                            on_picked(sk, ptr);
                    });
                }
            }
        }

    public:
#if defined(_DEBUG)
        struct map_streamer {
            std::ostream &_os;

            explicit map_streamer(std::istream &is)
                : _os(os) {}

            template<class K, class V>
            void operator()(std::pair<K, V> const &val) {
                // .first is your key, .second is your value
                _os << " - " << val.first << " : " << (*val.second) << "\n";
            }
        };

        template<class K, class V, class Comp = std::less<K>>
        inline void print_sorted(std::istream &is, std::unordered_map<K, V> const &um, Comp pred = Comp()) const {
            std::map<K, V> m(um.begin(), um.end(), pred);
            std::for_each(m.begin(), m.end(), map_streamer(os));
        }

        // template<class K, class V>
        // void print_sorted(std::unordered_map<K, V> const &um) {
        //     print_sorted(um, std::less<K>());
        // }

        void dump_full_keys(std::istream &is, const_chars leading_title = nullptr) const {
            if (leading_title) os << leading_title;
            else
                os << "Dumping for var_t ...";
            os << std::endl;

            // std::vector<std::string> keys;
            // keys.reserve(_full_key_map.size());
            // for (auto &it : _full_key_map) {
            //     keys.push_back(it.first);
            // }
            // std::sort(keys.begin(), keys.end());
            //
            // for (auto &it : keys) {
            //     os << " - " << it << " => " << _full_key_map[it] << std::endl;
            // }
            print_sorted(os, _full_key_map);
        }

        // template<class K, class V, class Comp = std::less<K>>
        void dump_tree(std::istream &is, const_chars leading_title = nullptr) const {
            if (leading_title) os << leading_title;
            else
                os << "Dumping for var_t as Tree ...";
            os << std::endl;

            dump_tree(os, _children, 0);
        }

    private:
        template<class K, class V>
        void dump_tree(std::istream &is, std::unordered_map<K, V> const &um, int level) const {
            for (auto const &[k, v] : um) {
                for (int i = 0; i < level; i++)
                    os << "  ";
                os << k << ": " << v << std::endl;
                dump_tree(os, v._children, level + 1);
            }
        }
#endif

    public:
        const self_type &null_elem() const {
            static self_type o;
            return o;
        }

        [[nodiscard]] bool is_empty() const { return (*this) == null_elem(); }

        holderT &value() { return _value; }
        const holderT &value() const { return _value; }

        template<class T>
        void format(std::istream &is, T const &val) {
            os << val;
        }

        friend std::ostream &operator<<(std::ostream &output, const var_t &v) {
            output << v._value;
            return output;
        }

        friend std::istream &operator>>(std::istream &input, var_t &v) {
            input >> v._value;
            return input;
        }

    private:
        // the saved entries of `app' looks like:
        //   server    array("tls", "port", "host", ...)
        //   client    array("tls", "server-addr", ...)
        //   logging   array("level", "target", ...)
        //   ...
        children_container _children;
        // the saved entries looks like:
        //   app.server         *var_t
        //   app.server.cert    *var_t
        //   app.client         *var_t
        //   app.logging        *var_t
        child_pointers _full_key_map;
        // any type data
        holderT _value;
        self_type *_parent{nullptr}; // need not to use std::weak_ptr
        // std::string _key_part;
    }; // class opt_var_t
#endif

#if 0
    template<typename holderT /* = streamable_any*/>
    class store_base {
    public:
        typedef var_t<holderT> var_type;

    public:
        store_base() = default;
        ~store_base() = default;

    public:
        holderT &get(const_chars key) {
            auto &it = _root.get(key);
            return it.value();
        }

        auto &get_var_t(const_chars key) { _root.get(key); }
        auto const &get_var_t(const_chars key) const { return _root.get(key); }

        void set(const_chars key, holderT &&val) { _root.set(key, val, &_root); }

        template<class Any>
        void set(const_chars key, Any &&val) {
            holderT v(val);
            _root.set(key, v, &_root);
        }

        template<class Any>
        void set(const_chars key, const Any &val) {
            holderT v(val);
            _root.set(key, v, &_root);
        }

    public:
        const var_type &root() const { return _root; }

        template<class T>
        void format(std::istream &is, T const &v) const {
            os << v;
        }

        friend std::ostream &operator<<(std::ostream &output, store_base const &v) {
            v.format(output, v._root);
            return output;
        }

        friend std::istream &operator>>(std::istream &input, store_base &v) {
            input >> v._root;
            return input;
        }

    private:
        var_type _root;
    }; // class store


    template<class V = support_types>
    using store = cmdr::opt::vars::store_base<V>;
#endif


    class store : public treeT<variable> {
    public:
        store() = default;
        ~store() = default;

        friend std::ostream &operator<<(std::ostream &os, store const &o) {
            o.dump_tree(os);
            return os;
        }
    }; // class store


} // namespace cmdr::vars

#endif //CMDR_CXX11_CMDR_VAR_T_HH
