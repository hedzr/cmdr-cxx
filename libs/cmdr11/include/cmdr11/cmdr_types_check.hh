//
// Created by Hedzr Yeh on 2021/1/20.
//

#ifndef CMDR_CXX11_CMDR_TYPES_CHECK_HH
#define CMDR_CXX11_CMDR_TYPES_CHECK_HH

#include <chrono>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef _MSC_VER
#include <cxxabi.h>
#endif
#include <cstdlib>
#include <memory>
#include <string>

namespace cmdr {

#if __cplusplus < 201402
    template<class T>
    std::string
    type_name() {
        typedef typename std::remove_reference<T>::type TR;
        std::unique_ptr<char, void (*)(void *)> own(
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                    nullptr, nullptr),
#else
                nullptr,
#endif
                std::free);
        std::string r = own != nullptr ? own.get() : typeid(TR).name();
        if (std::is_const<TR>::value)
            r += " const";
        if (std::is_volatile<TR>::value)
            r += " volatile";
        if (std::is_lvalue_reference<T>::value)
            r += "&";
        else if (std::is_rvalue_reference<T>::value)
            r += "&&";
        return r;
    }
#else
    template<typename T>
    constexpr auto type_name() noexcept {
        std::string_view name = "Error: unsupported compiler", prefix, suffix;
#ifdef __clang__
        name = __PRETTY_FUNCTION__;
        prefix = "auto type_name() [T = ";
        suffix = "]";
#elif defined(__GNUC__)
        name = __PRETTY_FUNCTION__;
        prefix = "constexpr auto type_name() [with T = ";
        suffix = "]";
#elif defined(_MSC_VER)
        name = __FUNCSIG__;
        prefix = "auto __cdecl type_name<";
        suffix = ">(void) noexcept";
#endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }
#endif


    // to detect the type of a lambda function, following:
    //   https://stackoverflow.com/a/7943736/6375060

    template<class>
    struct mem_type;

    template<class C, class T>
    struct mem_type<T C::*> {
        typedef T type;
    };

    template<class T>
    struct lambda_func_type {
        typedef typename mem_type<decltype(&T::operator())>::type type;
    };

#if 0
    void main_lambda_compare() {
        auto l = [](int i) { return long(i); };
        typedef lambda_func_type<decltype(l)>::type T;
        static_assert(std::is_same<T, long(int) const>::value, "ok");
    }
#endif


    // template<class T, typename=void>
    // struct is_duration :std::false_type {
    // };
    //
    // template<class T,
    //          typename std::enable_if<
    //                  std::is_same<std::decay<T>::type,
    //                                               std::duration<typename std::decay<T>::type::value_type,typename std::decay<T>::type::value_type>::type > >
    // struct is_duration<T> :std::true_type {
    // };

#if 0
    template<class T, int = 0>
    struct is_duration : std::false_type {};

    template<class Rep, class Period>
    struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

    template<class Rep, class Period>
    struct is_duration<const std::chrono::duration<Rep, Period>> : std::true_type {};

    template<class Rep, class Period>
    struct is_duration<volatile std::chrono::duration<Rep, Period>> : std::true_type {};

    template<class Rep, class Period>
    struct is_duration<const volatile std::chrono::duration<Rep, Period>> : std::true_type {};

    //template<class T, std::enable_if<std::is_same<std::decay<T>, std::chrono::seconds>::value, int>>
    //struct is_duration<T, 1> : std::true_type {};

    template<>
    struct is_duration<std::chrono::seconds> : std::true_type {};
#else

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

#endif

    //


    template<class T>
    struct is_vector {
        using type = T;
        constexpr static bool value = false;
    };

    template<class T>
    struct is_vector<std::vector<T>> {
        using type = std::vector<std::decay<T>>;
        constexpr static bool value = true;
    };

    // and the two "olbigatory" aliases
    //
    // Usages:
    //     std::cout << std::boolalpha;
    //     std::cout << is_vector_v<std::vector<int>> << std::endl ;
    //     std::cout << is_vector_v<int> << std::endl
    template<typename T>
    inline constexpr bool is_vector_v = is_vector<T>::value;

    template<typename T>
    using is_vector_t = typename is_vector<T>::type;


#if defined(ANOTHER_IS_VECTOR)
    template<typename T, typename _ = void>
    struct isVector : std::false_type {
    };

    template<typename T>
    struct isVector<T,
                    typename std::enable_if<
                            std::is_same<typename std::decay<T>::type,
                                         std::vector<typename std::decay<T>::type::value_type, typename std::decay<T>::type::allocator_type>>::value>::type> : std::true_type {
    };
#endif


    template<typename T, typename = void>
    struct is_container : std::false_type {};

    template<typename T>
    struct is_container<T, std::void_t<decltype(std::declval<T>().data()), decltype(std::declval<T>().size())>> : std::true_type {};

    //specialize a type for all of the STL containers.
    //
    // better than is_container & is_vector, see also:
    //   https://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time/31105859#31105859
    namespace is_stl_container_impl {
        template<typename T>
        struct is_stl_container : std::false_type {};
        template<typename T, std::size_t N>
        struct is_stl_container<std::array<T, N>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::vector<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::deque<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::list<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::forward_list<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::set<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::multiset<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::map<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::multimap<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::unordered_set<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::unordered_multiset<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::unordered_map<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::unordered_multimap<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::stack<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::queue<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::priority_queue<Args...>> : std::true_type {};
    } // namespace is_stl_container_impl

    //type trait to utilize the implementation type traits as well as decay the type
    template<typename T>
    struct is_stl_container {
        static constexpr bool const value = is_stl_container_impl::is_stl_container<T>::value;
    };


    // https://stackoverflow.com/questions/12042824/how-to-write-a-type-trait-is-container-or-is-vector
    template<typename T, typename _ = void>
    struct is_generic_container : std::false_type {};

    template<typename... Ts>
    struct is_generic_container_helper {};

    template<typename T>
    struct is_generic_container<
            T,
            std::conditional_t<
                    false,
                    is_generic_container_helper<
                            typename T::value_type,
                            typename T::size_type,
                            typename T::allocator_type,
                            typename T::iterator,
                            typename T::const_iterator,
                            decltype(std::declval<T>().size()),
                            decltype(std::declval<T>().begin()),
                            decltype(std::declval<T>().end()),
                            decltype(std::declval<T>().cbegin()),
                            decltype(std::declval<T>().cend())>,
                    void>> : public std::true_type {};


    // SFINAE assertion class here: is_streamable
    // original: https://stackoverflow.com/questions/22758291/how-can-i-detect-if-a-type-can-be-streamed-to-an-stdostream
    template<typename T, typename S = std::ostream>
    class is_streamable {
        template<typename SS, typename TT>
        static auto test(int)
                -> decltype(std::declval<SS &>() << std::declval<TT>(), std::true_type());

        template<typename, typename>
        static auto test(...) -> std::false_type;

    public:
        static const bool value = decltype(test<S, T>(0))::value;
    };


} // namespace cmdr


#endif //CMDR_CXX11_CMDR_TYPES_CHECK_HH
