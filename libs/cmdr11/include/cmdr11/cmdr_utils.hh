//
// Created by Hedzr Yeh on 2021/1/13.
//

#ifndef CMDR_CXX11_CMDR_UTILS_HH
#define CMDR_CXX11_CMDR_UTILS_HH

#include <any>
#include <optional>
#include <variant>

#include <algorithm>
#include <functional>
#include <utility>

#include <cassert>
#include <stdexcept>

#include <iomanip>
#include <iostream>
#include <sstream>

#include <string>

#include <unordered_map>
#include <unordered_set>

#include <type_traits>
#include <typeindex>
#include <typeinfo>

#include <memory>

#include <chrono>

#include "cmdr_dbg.hh"

#include "cmdr_chrono.hh"


namespace cmdr::util {

    //
#if 0
    template<typename T>
    class Nullable {
    public:
        Nullable() = default;

        virtual ~Nullable() {
            if (_value) delete _value;
        }

    public:
        Nullable(const Nullable &o) { _copy(o); }

        Nullable &operator=(const Nullable &o) {
            _copy(o);
            return *this;
        }

        Nullable &operator=(const T &o) {
            this->_value = o;
            return *this;
        }

    private:
        void _copy(const Nullable &o) {
            this->_value = o._value;
        }

    public:
        T &val() { return *_value; }

        const T &val() const { return *_value; }

        void val(T &&v) {
            if (!_value)
                _value = new T;
            (*_value) = v;
        }

        explicit operator T() const { return val(); }

        explicit operator T() { return val(); }

        // operator ->
        // operator *

        // operator ==, !=, >=, <=, >, <, <=>
        // operator +,-,*,/,...

        [[nodiscard]] bool is_null() const { return !_value; }

    private:
        T *_value{nullptr};
    }; // class Nullable<T>
#endif


#if 0
    template<class OS = std::ostream>
    class streamer_any {
    public:
        // OS &os;
        typedef std::unordered_map<std::type_index, std::function<void(std::ostream &os, std::any const &)>> R;
        R any_visitors;

        streamer_any()
            : any_visitors{
                      to_any_visitor<void>([](std::ostream &os) { os << "{}"; }),
                      to_any_visitor<bool>([](std::ostream &os, bool x) { os << std::boolalpha << x; }),
                      to_any_visitor<int>([](std::ostream &os, int x) { os << x; }),
                      to_any_visitor<int8_t>([](std::ostream &os, int8_t x) { os << x; }),
                      to_any_visitor<int16_t>([](std::ostream &os, int16_t x) { os << x; }),
                      to_any_visitor<int32_t>([](std::ostream &os, int32_t x) { os << x; }),
                      to_any_visitor<int64_t>([](std::ostream &os, int64_t x) { os << x; }),
                      to_any_visitor<unsigned>([](std::ostream &os, unsigned x) { os << x; }),
                      to_any_visitor<uint8_t>([](std::ostream &os, uint8_t x) { os << x; }),
                      to_any_visitor<uint16_t>([](std::ostream &os, uint16_t x) { os << x; }),
                      to_any_visitor<uint32_t>([](std::ostream &os, uint32_t x) { os << x; }),
                      to_any_visitor<uint64_t>([](std::ostream &os, uint64_t x) { os << x; }),
                      to_any_visitor<long>([](std::ostream &os, long x) { os << x; }),
                      to_any_visitor<unsigned long>([](std::ostream &os, unsigned long x) { os << x; }),
                      to_any_visitor<float>([](std::ostream &os, float x) { os << x; }),
                      to_any_visitor<double>([](std::ostream &os, double x) { os << x; }),
                      to_any_visitor<char const *>([](std::ostream &os, char const *s) { os << std::quoted(s); }),
                      to_any_visitor<std::chrono::nanoseconds>([](std::ostream &os, const std::chrono::nanoseconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::microseconds>([](std::ostream &os, const std::chrono::microseconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::milliseconds>([](std::ostream &os, const std::chrono::milliseconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::seconds>([](std::ostream &os, const std::chrono::seconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::minutes>([](std::ostream &os, const std::chrono::minutes &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::hours>([](std::ostream &os, const std::chrono::hours &x) { cmdr::chrono::format_duration(os, x); }),
                      // ... add more handlers for your types ...
              } {}


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

        inline void process(std::ostream &os, const std::any &a) {
            if (const auto it = any_visitors.find(std::type_index(a.type()));
                it != any_visitors.cend()) {
                it->second(os, a);
            } else {
                std::cout << "Unregistered type " << std::quoted(a.type().name());
            }
        }

        template<class T, class F>
        inline void register_any_visitor(F const &f) {
            std::cout << "Register visitor for type "
                      << std::quoted(typeid(T).name()) << '\n';
            any_visitors.insert(to_any_visitor<T>(f));
        }
    };
#endif


    template<class Visitor>
    inline void visit_any_as(std::any const &, Visitor &&) {
        cmdr_throw_line("std::any contained no suitable type, unable to visit");
    }

    template<class First, class... Rest, class Visitor>
    inline void visit_any_as(std::any const &any, Visitor &&visitor) {
        First const *value = std::any_cast<First>(&any);
        if (value) {
            visitor(*value);
        } else {
            visit_any_as<Rest...>(any, std::forward<Visitor>(visitor));
        }
    }


    template<typename T>
    class singleton {
    public:
        static T &instance();

        singleton(const singleton &) = delete;
        singleton &operator=(const singleton) = delete;

    protected:
        struct token {};
        singleton() = default;
    };

    template<typename T>
    inline T &singleton<T>::instance() {
        static const std::unique_ptr<T> instance{new T{token{}}};
        return *instance;
    }


    template<typename C, typename... Args>
    class singleton_with_optional_construction_args {
    private:
        singleton_with_optional_construction_args() = default;
        static C *_instance;

    public:
        ~singleton_with_optional_construction_args() {
            delete _instance;
            _instance = nullptr;
        }
        static C &instance(Args... args) {
            if (_instance == nullptr)
                _instance = new C(args...);
            return *_instance;
        }
    };

    template<typename C, typename... Args>
    C *singleton_with_optional_construction_args<C, Args...>::_instance = nullptr;

#if defined(_DEBUG) && defined(NEVER_USED)
    inline void test_singleton_with_optional_construction_args() {
        int &i = singleton_with_optional_construction_args<int, int>::instance(1);
        UTEST_CHECK(i == 1);

        tester1 &t1 = singleton_with_optional_construction_args<tester1, int>::instance(1);
        UTEST_CHECK(t1.result() == 1);

        tester2 &t2 = singleton_with_optional_construction_args<tester2, int, int>::instance(1, 2);
        UTEST_CHECK(t2.result() == 3);
    }
#endif // defined(NEVER_USED)

} //namespace cmdr::util

#endif //CMDR_CXX11_CMDR_UTILS_HH
