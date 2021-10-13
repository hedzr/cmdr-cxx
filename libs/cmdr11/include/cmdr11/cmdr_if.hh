//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef _PRIVATE_VAR_FOLDERS_0K_1RQY3K4X7_5B_73SW5PY2BW00000GN_T_CLION_CLANG_TIDY_CMDR_IF_HH
#define _PRIVATE_VAR_FOLDERS_0K_1RQY3K4X7_5B_73SW5PY2BW00000GN_T_CLION_CLANG_TIDY_CMDR_IF_HH

#include <type_traits>

#if (__cplusplus > 201402L)
#include <experimental/type_traits>
#endif

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
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>


#if !defined(__TRAITS_VOIT_T_DEFINED)
#define __TRAITS_VOIT_T_DEFINED
// ------------------------- void_t
namespace cmdr::traits {
#if (__cplusplus > 201402L)
    using std::void_t; // C++17 or later
#else
    // template<class...>
    // using void_t = void;

    template<typename... T>
    struct make_void { using type = void; };
    template<typename... T>
    using void_t = typename make_void<T...>::type;
#endif
} // namespace cmdr::traits
#endif // __TRAITS_VOIT_T_DEFINED


#if !defined(__TRAITS_IS_DETECTED_DEFINED)
#define __TRAITS_IS_DETECTED_DEFINED
// ------------------------- is_detected
namespace cmdr::traits {
    template<class, template<class> class, class = void_t<>>
    struct detect : std::false_type {};

    template<class T, template<class> class Op>
    struct detect<T, Op, void_t<Op<T>>> : std::true_type {};

    template<class T, class Void, template<class...> class Op, class... Args>
    struct detector {
        using value_t = std::false_type;
        using type = T;
    };

    template<class T, template<class...> class Op, class... Args>
    struct detector<T, void_t<Op<Args...>>, Op, Args...> {
        using value_t = std::true_type;
        using type = Op<Args...>;
    };

    struct nonesuch final {
        nonesuch() = delete;
        ~nonesuch() = delete;
        nonesuch(const nonesuch &) = delete;
        void operator=(const nonesuch &) = delete;
    };

    template<class T, template<class...> class Op, class... Args>
    using detected_or = detector<T, void, Op, Args...>;

    template<class T, template<class...> class Op, class... Args>
    using detected_or_t = typename detected_or<T, Op, Args...>::type;

    template<template<class...> class Op, class... Args>
    using detected = detected_or<nonesuch, Op, Args...>;

    template<template<class...> class Op, class... Args>
    using detected_t = typename detected<Op, Args...>::type;

    /**
     * @brief another std::is_detected
     * @details For example:
     * @code{c++}
     * template&lt;typename T>
     * using copy_assign_op = decltype(std::declval&lt;T &>() = std::declval&lt;const T &>());
     * 
     * template&lt;typename T>
     * using is_copy_assignable = is_detected&lt;copy_assign_op, T>;
     * 
     * template&lt;typename T>
     * constexpr bool is_copy_assignable_v = is_copy_assignable&lt;T>::value;
     * @endcode
     */
    template<template<class...> class Op, class... Args>
    using is_detected = typename detected<Op, Args...>::value_t;

    template<template<class...> class Op, class... Args>
    constexpr bool is_detected_v = is_detected<Op, Args...>::value;

    template<class T, template<class...> class Op, class... Args>
    using is_detected_exact = std::is_same<T, detected_t<Op, Args...>>;

    template<class To, template<class...> class Op, class... Args>
    using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

#if 0
    // Proposing Standard Library Support for the C++ Detection Idiom, v2
    // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf
    // and, https://gist.github.com/remyroez/6a6ac795452f8ec32b6d617bd6c8d946

    // usage ----------

    template<typename T>
    using copy_assign_op = decltype(std::declval<T &>() = std::declval<const T &>());

    template<typename T>
    using is_copy_assignable = is_detected<copy_assign_op, T>;

    template<typename T>
    constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

    struct foo {};
    struct bar {
        bar &operator=(const bar &) = delete;
    };

    int main() {
        static_assert(is_copy_assignable_v<foo>, "foo is copy assignable");
        static_assert(!is_copy_assignable_v<bar>, "bar is not copy assignable");
        return 0;
    }
#endif

} // namespace cmdr::traits
#endif // __TRAITS_IS_DETECTED_DEFINED


// ------------------------- has_string
namespace cmdr::traits {
    /**
     * @brief test for the prototype: `std::string to_string()`
     * @tparam T 
     * @details For example:
     * @code{c++}
     * struct with_string { std::string to_string() const { return ""; } };
     * struct wrong_string { const char *to_string() const { return ""; } };
     * 
     * std::cout &lt;&lt; '\n'
     *         &lt;&lt; has_string&lt;int&gt;::value &lt;&lt; '\n'
     *         &lt;&lt; has_string&lt;with_string&gt;::value &lt;&lt; '\n'
     *         &lt;&lt; has_string&lt;wrong_string&gt;::value &lt;&lt; '\n';
     * @endcode
     */
    template<typename T>
    class has_string {
        template<typename U>
        static constexpr std::false_type test(...) { return {}; }
        template<typename U>
        static constexpr auto test(U *u) ->
                typename std::is_same<std::string, decltype(u->to_string())>::type { return {}; }

    public:
        static constexpr bool value = test<T>(nullptr);
    };


    template<typename T>
    using toString_t = decltype(std::declval<T &>().toString());

    /**
     * @brief test for the prototype: `std::string toString()`
     * @tparam T 
     */
    template<typename T>
    constexpr bool has_toString = is_detected_v<toString_t, T>;

    template<typename T>
    using to_string_t = decltype(std::declval<T &>().to_string());

    /**
     * @brief test for the prototype: `std::string to_string()`
     * @tparam T 
     */
    template<typename T>
    constexpr bool has_to_string = is_detected_v<to_string_t, T>;

} // namespace cmdr::traits

// ------------------------- has_member
namespace cmdr::traits {
    template<typename T, typename F>
    constexpr auto has_member_impl(F &&f) -> decltype(f(std::declval<T>()), true) { return true; }

    template<typename>
    constexpr bool has_member_impl(...) { return false; }

    /**
     * @brief 
     * @details For example:
     * @code{c++}
     * template&lt;class T&gt;
     * std::string optionalToString(T* obj) {
     *     if constexpr(has_member(T, toString()))
     *         return obj->toString();
     *     else
     *         return "toString not defined";
     * }
     * 
     * @endcode
     */
#define has_member(T, EXPR) has_member_impl<T>([](auto &&obj) -> decltype(obj.EXPR) {})
} // namespace cmdr::traits

// ------------------------- has_begin/end_v, has_subscript_v,
// has_push/pop_v, has_top(_func)_v,
// has_emplace_back_v, has_emplace_v, has_push/pop_back_v,
// has_size_v, has_empty_v,
namespace cmdr::traits {

    template<typename T, typename = void>
    struct has_begin : std::false_type {};

    template<typename T>
    struct has_begin<T, decltype(void(std::declval<T &>().begin()))> : std::true_type {};

    /**
     * @brief test for member function: `RET begin()`
     * @tparam T 
     * @details For example:
     * @code{c++}
     * static_assert(undo_cxx::traits::has_begin&lt;std::string>::value);
     * 
     * void test_begin() {
     *     if constexpr (undo_cxx::traits::has_begin_v&lt;Container>) {
     *         std::cout &lt;&lt; "M: begin() exists." &lt;&lt; '\n';
     *     } else {
     *         std::cout &lt;&lt; "M: begin() not exists." &lt;&lt; '\n';
     *     }
     * }
     * @endcode
     */
    template<typename T>
    constexpr bool has_begin_v = has_begin<T>::value;

    template<typename T, typename = void>
    struct has_end : std::false_type {};

    template<typename T>
    struct has_end<T, decltype(void(std::declval<T &>().end()))> : std::true_type {};

    template<typename T>
    constexpr bool has_end_v = has_end<T>::value;


    /**
     * @brief test subscript operator `[]`
     * @code{c++}
     * using a = subscript_t&lt; std::vector&lt;int>, size_t >;
     * using b = subscript_t&lt; std::vector&lt;int>, int >;
     * @endcode
     */
    // template<typename T, typename Index>
    // using subscript_t = decltype(std::declval<T>()[std::declval<Index>()]);
    //
    // template<class T, class Index = std::size_t, class = void>
    // constexpr bool has_op_subscript{false};
    //
    // template<class T, class Index = std::size_t>
    // constexpr bool has_op_subscript<T, void_t<subscript_t<T,Index>>>{true};

#if defined(__clang__) // gcc 10.x failed, 11.x ok; msvc failed;
    template<typename T, typename Ret, typename Index>
    using subscript_t = std::integral_constant < Ret (T::*)(Index),
          &T::operator[]>;

    template<typename T, typename Ret, typename Index>
    using has_subscript = is_detected<subscript_t, T, Ret, Index>;

    template<typename T, typename Ret, typename Index>
    constexpr bool has_subscript_v = has_subscript<T, Ret, Index>::value;

#if !defined(__GNUC__) // gcc 10.x failed, 11.x ok,
    static_assert(has_subscript<std::vector<int>, int &, size_t>::value == true);
#endif
    static_assert(!has_subscript<std::vector<int>, int &, int>::value);
#endif


    // template <typename T, typename = void>
    // struct has_push : std::false_type {};
    //
    // template <typename T, typename ...Args>
    // struct has_push<T, decltype(void(std::declval<T &>().push()))> : std::true_type {};

    // C++20
    //
    // template<typename Callable, typename... Args, typename = decltype(declval<Callable>()(declval<Args>()...))>
    // std::true_type is_callable_impl(Callable, Args...) { return {}; }
    //
    // std::false_type is_callable_impl(...) { return {}; }
    //
    // template<typename... Args, typename Callable>
    // constexpr bool is_callable(Callable callable) {
    //     return decltype(is_callable_impl(callable, declval<Args>()...)){};
    // }

    // C++20
    //
    // template<typename T>
    // concept has_toString = requires(const T &t) {
    //     t.toString();
    // };
    //
    // template<typename T>
    // std::string optionalToString(const T &obj) {
    //     if constexpr (has_toString<T>)
    //         return obj.toString();
    //     else
    //         return "toString not defined";
    // }

    // C++17 with <experimental/type_traits>
    //
    // template<typename T>
    // using toString_t = decltype(std::declval<T &>().toString());
    //
    // template<typename T>
    // constexpr bool has_toString = std::is_detected_v<toString_t, T>;

    template<class T, class = void>
    constexpr bool has_push_v{false};

    template<class T>
    constexpr bool has_push_v<T, void_t<decltype(std::declval<T>().push(std::declval<typename std::decay_t<T>::value_type>()))>>{true};


    template<class T, class = void>
    constexpr bool has_pop_v{false};

    template<class T>
    constexpr bool has_pop_v<T, void_t<decltype(std::declval<T>().pop())>>{true};

    template<typename T>
    struct has_top_func {
        template<typename U>
        static constexpr decltype(std::declval<U>().top(), bool())
        tester(int) { return true; }

        template<typename U>
        static constexpr bool tester(...) { return false; }

        static constexpr bool value = tester<T>(int());
    };

    template<class T, class = void>
    constexpr bool has_top_v{false};

    template<class T>
    constexpr bool has_top_v<T, void_t<decltype(std::declval<T>().top())>>{true};


    // template<class T, class = void>
    // constexpr bool has_emplace{false};
    //
    // template<class T>
    // constexpr bool has_emplace<T, void_t<decltype(std::declval<T>().has_emplace(std::declval<typename std::decay_t<T>::value_type>()))>>{true};

    template<typename T, typename = void>
    struct has_emplace_back : std::false_type {};

    template<typename T>
    struct has_emplace_back<T, void_t<decltype(std::declval<T>().emplace_back(std::declval<typename T::value_type>()))>> : std::true_type {};

    template<typename T>
    constexpr bool has_emplace_back_v = has_emplace_back<T>::value;

    template<typename T, typename = void>
    struct has_emplace : std::false_type {};

    template<typename T>
    struct has_emplace<T, void_t<decltype(std::declval<T>().emplace(std::declval<typename T::value_type>()))>> : std::true_type {};

    template<typename T>
    constexpr bool has_emplace_v = has_emplace<T>::value;

    // template<typename T, typename = void>
    // struct has_emplace_variadic : std::false_type {};
    //
    // /**
    //  * @brief not yet!!
    //  * @tparam T
    //  */
    // template<typename T>
    // struct has_emplace_variadic<T, void_t<decltype(std::declval<T>().emplace(std::declval<typename T::value_type>()))>> : std::true_type {};


    template<class T, typename... Arguments>
    using emplace_variadic_t = std::conditional_t<
            true,
            decltype(std::declval<T>().emplace(std::declval<Arguments>()...)),
            std::integral_constant<
                    decltype(std::declval<T>().emplace(std::declval<Arguments>()...)) (T::*)(Arguments...),
                    &T::emplace>>;
    /**
     * @brief test member function `emplace()` with variadic params
     * @tparam T 
     * @tparam Arguments 
     * @details For example:
     * @code{c++}
     * using C = std::list&lt;int>;
     * static_assert(has_emplace_variadic_v&lt;C, C::const_iterator, int &&>);
     * @endcode
     */
    template<class T, typename... Arguments>
    constexpr bool has_emplace_variadic_v = is_detected_v<emplace_variadic_t, T, Arguments...>;
    namespace detail {
        using C = std::list<int>;
        static_assert(has_emplace_variadic_v<C, C::const_iterator, int &&>);
    } // namespace detail


    //

    //


    template<class T, class = void>
    constexpr bool has_push_back_v{false};

    template<class T>
    constexpr bool has_push_back_v<T, void_t<decltype(std::declval<T>().push_back(std::declval<typename std::decay_t<T>::value_type>()))>>{true};


    template<class T, class = void>
    constexpr bool has_pop_back_v{false};

    template<class T>
    constexpr bool has_pop_back_v<T, void_t<decltype(std::declval<T>().pop_back())>>{true};


    template<class T, class = void>
    constexpr bool has_size_v{false};

    template<class T>
    constexpr bool has_size_v<T, void_t<decltype(std::declval<T>().size())>>{true};


    template<class T, class = void>
    constexpr bool has_empty_v{false};

    template<class T>
    constexpr bool has_empty_v<T, void_t<decltype(std::declval<T>().empty())>>{true};

#if 0
    namespace detail {
        
        template<typename CIn, typename COut, std::enable_if_t<has_push_back<COut>, bool> = true>
        void addAll(CIn &&from, COut &&to) {
            std::copy(std::begin(from), std::end(from), std::back_inserter(to));
        }

        template<typename CIn, typename COut, std::enable_if_t<!has_push_back<COut>, bool> = true>
        void addAll(CIn &&from, COut &&to) {
            std::copy(std::begin(from), std::end(from), std::inserter(to, to.begin()));
        }
    } // namespace detail
#endif

} // namespace cmdr::traits


// ------------------------- is_duration, is_vector
// is_container, is_stl_container, is_generic_container,
// is_streamable,
namespace cmdr::traits {


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
    //     std::cout << is_vector_v<std::vector<int>> << '\n' ;
    //     std::cout << is_vector_v<int> << '\n'
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


} // namespace cmdr::traits


#if 0

//
// http://coliru.stacked-crooked.com/a/ff9a456aaee6ec47
// https://stackoverflow.com/questions/40439909/c98-03-stdis-constructible-implementation
// https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(fontScale:14,j:1,lang:c%2B%2B,selection:(endColumn:2,endLineNumber:20,positionColumn:1,positionLineNumber:2,selectionStartColumn:2,selectionStartLineNumber:20,startColumn:1,startLineNumber:2),source:%27%23define+_LIBCPP_TESTING_FALLBACK_IS_CONSTRUCTIBLE%0A%23include+%3Ctype_traits%3E%0A%0Astruct+D1%3B%0Astruct+D2%3B%0Astruct+Base+%7B%0A++++explicit+operator+D1%26()%3B%0A%7D%3B%0A%0Astruct+D1+:+Base+%7B%0A++++D1(const+D1%26)+%3D+delete%3B%0A%7D%3B%0Astruct+D2+:+Base+%7B%7D%3B%0A%0Aint+BtoD1()+%7B%0A++++return+std::is_constructible%3CD1%26,+Base%26%3E::value%3B%0A%7D%0Aint+BtoD2()+%7B%0A++++return+std::is_constructible%3CD2%26,+Base%26%3E::value%3B%0A%7D%0A%0A%23if+defined(__clang__)+%7C%7C+(__GNUC__+%3E%3D+8)%0Aint+BtoD1_builtin()+%7B%0A++++return+__is_constructible(D1%26,+Base%26)%3B%0A%7D%0Aint+BtoD2_builtin()+%7B%0A++++return+__is_constructible(D2%26,+Base%26)%3B%0A%7D%0A%23endif%0A%0A%23ifdef+_LIBCPP_VERSION%0Aint+BtoD1_lib()+%7B%0A++++return+std::__libcpp_is_constructible%3CD1%26,+Base%26%3E::value%3B%0A%7D%0Aint+BtoD2_lib()+%7B%0A++++return+std::__libcpp_is_constructible%3CD2%26,+Base%26%3E::value%3B%0A%7D%0A%23endif%27),l:%275%27,n:%270%27,o:%27C%2B%2B+source+%231%27,t:%270%27)),k:50,l:%274%27,n:%270%27,o:%27%27,s:0,t:%270%27),(g:!((g:!((h:compiler,i:(compiler:clang400,filters:(b:%270%27,binary:%271%27,commentOnly:%270%27,demangle:%270%27,directives:%270%27,execute:%271%27,intel:%270%27,libraryCode:%271%27,trim:%271%27),fontScale:14,j:1,lang:c%2B%2B,libs:!(),options:%27-O2+-std%3Dc%2B%2B11+-stdlib%3Dlibc%2B%2B%27,selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:%275%27,n:%270%27,o:%27x86-64+clang+4.0.0+(Editor+%231,+Compiler+%231)+C%2B%2B%27,t:%270%27)),k:50,l:%274%27,m:68.14159292035397,n:%270%27,o:%27%27,s:0,t:%270%27),(g:!((h:output,i:(compiler:1,editor:1,fontScale:14,wrap:%271%27),l:%275%27,n:%270%27,o:%27%231+with+x86-64+clang+4.0.0%27,t:%270%27)),header:(),l:%274%27,m:31.85840707964602,n:%270%27,o:%27%27,s:0,t:%270%27)),k:50,l:%273%27,n:%270%27,o:%27%27,t:%270%27)),l:%272%27,n:%270%27,o:%27%27,t:%270%27)),version:4
//
namespace hicc::traits {

#if __GNUC__ < 10

    template<typename T, T Val>
    struct integral_constant {
        typedef integral_constant type;
        typedef T value_type;
        enum {
            value = Val
        };
    };

    typedef integral_constant<bool, true> true_type;
    typedef integral_constant<bool, false> false_type;

    template<typename T>
    struct remove_ref {
        typedef T type;
    };

    template<typename T>
    struct remove_ref<T &> {
        typedef T type;
    };

    // is_base_of from https://stackoverflow.com/questions/2910979/how-does-is-base-of-work
    namespace aux {
        typedef char yes[1];
        typedef char no[2];

        template<typename B, typename D>
        struct Host {
            operator B *() const;
            operator D *();
        };
    } // namespace aux
    template<typename B, typename D>
    struct is_base_of {
        template<typename T>
        static aux::yes &check(D *, T);
        static aux::no &check(B *, int);

        static const bool value = sizeof(check(aux::Host<B, D>(), int())) == sizeof(aux::yes);
    };

    template<typename T>
    struct remove_cv {
        typedef T type;
    };
    template<typename T>
    struct remove_cv<const T> {
        typedef T type;
    };
    template<typename T>
    struct remove_cv<volatile T> {
        typedef T type;
    };
    template<typename T>
    struct remove_cv<const volatile T> {
        typedef T type;
    };

    template<typename T>
    struct is_void : integral_constant<bool, false> {};
    template<>
    struct is_void<void> : integral_constant<bool, true> {};

    template<bool, typename T, typename>
    struct conditional {
        typedef T type;
    };
    template<typename T, typename U>
    struct conditional<false, T, U> {
        typedef U type;
    };


    namespace aux {

        template<typename T, typename U>
        struct is_more_const : integral_constant<bool, false> {};

        template<typename T, typename U>
        struct is_more_const<const T, U> : integral_constant<bool, true> {};

        template<typename T, typename U>
        struct is_more_const<const T, const U> : integral_constant<bool, false> {};

        template<typename T, typename U>
        struct is_more_volatile : integral_constant<bool, false> {};

        template<typename T, typename U>
        struct is_more_volatile<volatile T, U> : integral_constant<bool, true> {};

        template<typename T, typename U>
        struct is_more_volatile<volatile T, volatile U> : integral_constant<bool, false> {};

        template<typename T, typename U>
        struct is_more_cv : integral_constant<bool, is_more_const<T, U>::value && is_more_volatile<T, U>::value> {};


        template<typename T>
        struct is_default_constructible {
            template<typename U>
            static yes &test(int (*)[sizeof(new U)]);
            template<typename U>
            static no &test(...);
            enum {
                value = sizeof(test<T>(0)) == sizeof(yes)
            };
        };

        template<typename T, typename Arg>
        struct is_constructible_1 {
            template<typename U, typename Arg_>
            static yes &test(int (*)[sizeof(U(static_cast<Arg_>(*((typename remove_ref<Arg_>::type *) 0))))]);
            template<typename U, typename Arg_>
            static no &test(...);
            enum {
                value = sizeof(test<T, Arg>(0)) == sizeof(yes)
            };
        };

        // Base pointer construct from Derived Pointer
        template<typename T, typename U>
        struct is_constructible_1<T *, U *>
            : conditional<
                      is_void<typename remove_cv<T>::type>::value,
                      integral_constant<bool, true>,
                      typename conditional<
                              is_void<typename remove_cv<U>::type>::value,
                              integral_constant<bool, false>,
                              typename conditional<
                                      is_more_cv<T, U>::value,
                                      integral_constant<bool, false>,
                                      is_base_of<T, U>>::type>::type>::type {};

        // Base pointer construct from Derived Pointer
        template<typename T, typename U>
        struct is_constructible_1<T &, U &>
            : conditional<
                      is_more_cv<T, U>::value,
                      integral_constant<bool, false>,
                      is_base_of<T, U>>::type {};


        template<typename T, typename Arg1, typename Arg2>
        struct is_constructible_2 {
            template<typename U, typename Arg1_, typename Arg2_>
            static yes &test(int (*)[sizeof(U(
                    static_cast<Arg1_>(*((typename remove_ref<Arg1_>::type *) 0)),
                    static_cast<Arg2_>(*((typename remove_ref<Arg2_>::type *) 0))))]);
            template<typename U, typename Arg1_, typename Arg2_>
            static no &test(...);
            enum {
                value = sizeof(test<T, Arg1, Arg2>(0)) == sizeof(yes)
            };
        };
    } // namespace aux

    template<typename T, typename Arg1 = void, typename Arg2 = void>
    struct is_constructible : integral_constant<bool, aux::is_constructible_2<T, Arg1, Arg2>::value> {
    };

    template<typename T, typename Arg>
    struct is_constructible<T, Arg> : integral_constant<bool, aux::is_constructible_1<T, Arg>::value> {
    };
    template<typename T>
    struct is_constructible<T> : integral_constant<bool, aux::is_default_constructible<T>::value> {
    };

#else
    using is_constructible = std::is_constructible;
#endif

} // namespace hicc::traits

#endif // 0

#endif // _PRIVATE_VAR_FOLDERS_0K_1RQY3K4X7_5B_73SW5PY2BW00000GN_T_CLION_CLANG_TIDY_CMDR_IF_HH
