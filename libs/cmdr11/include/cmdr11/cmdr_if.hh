//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef _PRIVATE_VAR_FOLDERS_0K_1RQY3K4X7_5B_73SW5PY2BW00000GN_T_CLION_CLANG_TIDY_CMDR_IF_HH
#define _PRIVATE_VAR_FOLDERS_0K_1RQY3K4X7_5B_73SW5PY2BW00000GN_T_CLION_CLANG_TIDY_CMDR_IF_HH

#include <type_traits>

#include <list>
#include <vector>

#if (__cplusplus > 201402L)
#include <experimental/type_traits>
#endif


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

#define __TRAITS_IS_DETECTED_DEFINED

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

    template<typename T, typename Ret, typename Index>
    using subscript_t = std::integral_constant < Ret (T::*)(Index),
          &T::operator[]>;

    template<typename T, typename Ret, typename Index>
    using has_subscript = is_detected<subscript_t, T, Ret, Index>;

    template<typename T, typename Ret, typename Index>
    constexpr bool has_subscript_v = has_subscript<T, Ret, Index>::value;

    static_assert(has_subscript<std::vector<int>, int &, std::size_t>::value);
    static_assert(!has_subscript<std::vector<int>, int &, int>::value);


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

    template<typename T, typename = void>
    struct has_emplace_variadic : std::false_type {};

    /**
     * @brief not yet!!
     * @tparam T 
     */
    template<typename T>
    struct has_emplace_variadic<T, void_t<decltype(std::declval<T>().emplace(std::declval<typename T::value_type>()))>> : std::true_type {};


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

#endif // _PRIVATE_VAR_FOLDERS_0K_1RQY3K4X7_5B_73SW5PY2BW00000GN_T_CLION_CLANG_TIDY_CMDR_IF_HH
