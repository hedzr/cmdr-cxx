//
// Created by Hedzr Yeh on 2021/1/20.
//

#ifndef CMDR_CXX11_VISIT_ANY_HH
#define CMDR_CXX11_VISIT_ANY_HH

#include <any>
#include <functional>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "cmdr11/cmdr_types.hh"


namespace study {

    // SFINAE assertion class here: is_streamable
    // original: https://stackoverflow.com/questions/22758291/how-can-i-detect-if-a-type-can-be-streamed-to-an-stdostream
    template<typename T, typename S = std::ostream>
    class is_streamable_old {
        template<typename SS, typename TT>
        static auto test(int)
                -> decltype(std::declval<SS &>() << std::declval<TT>(), std::true_type());

        template<typename, typename>
        static auto test(...) -> std::false_type;

    public:
        static const bool value = decltype(test<S, T>(0))::value;
    };

    template<typename TT, typename SS>
    class is_streamable {
        // c++17
        template<typename T, typename S, typename = void>
        struct test : std::false_type {};

        // c++17
        template<typename T, typename S>
        struct test<T, S, std::void_t<decltype(std::declval<S &>() << std::declval<T>())>>
            : std::true_type {};

    public:
        static const bool value = decltype(test<TT, SS>(0))::value;
    };


    template<class OS = std::ostream>
    class streamer_any {
    public:
        // OS &os;
        typedef std::unordered_map<std::type_index, std::function<void(std::ostream &os, std::any const &)>> R;
        R any_visitors;
        streamer_any()
            : any_visitors{
                      to_any_visitor<void>([](std::ostream &os) { os << "{}"; }),
                      to_any_visitor<int>([](std::ostream &os, int x) { os << x; }),
                      to_any_visitor<unsigned>([](std::ostream &os, unsigned x) { os << x; }),
                      to_any_visitor<float>([](std::ostream &os, float x) { os << x; }),
                      to_any_visitor<double>([](std::ostream &os, double x) { os << x; }),
                      to_any_visitor<char const *>([](std::ostream &os, char const *s) { os << std::quoted(s); }),
                      // ... add more handlers for your types ...
                      to_any_visitor<std::chrono::nanoseconds>([](std::ostream &os, const std::chrono::nanoseconds &x) { cmdr::chrono::format_duration(os, x); }),
                      to_any_visitor<std::chrono::seconds>([](std::ostream &os, const std::chrono::seconds &x) { cmdr::chrono::format_duration(os, x); }),
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

} // namespace study


#endif //CMDR_CXX11_VISIT_ANY_HH
