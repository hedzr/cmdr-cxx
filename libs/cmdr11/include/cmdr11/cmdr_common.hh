//
// Created by Hedzr Yeh on 2021/8/31.
//

#ifndef CMDR_CXX_CMDR_COMMON_HH
#define CMDR_CXX_CMDR_COMMON_HH

#include "cmdr_defs.hh"

#include <algorithm>
#include <functional>
#include <memory>
#include <new>
#include <random>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>

#include <atomic>
#include <condition_variable>
#include <mutex>

#include <any>
#include <array>
#include <chrono>
#include <deque>
#include <initializer_list>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring> // std::strcmp, ...
#include <iomanip>
#include <iostream>
#include <string>

#include <math.h>

namespace std {

    template<typename T>
    inline unique_ptr<T> to_unique(T *ptr) { return unique_ptr<T>(ptr); }
    template<typename T>
    inline unique_ptr<T> to_unique(shared_ptr<T> &&ptr) {
        auto p = ptr.get();
        ptr.reset();
        auto pnew = unique_ptr<T>(p);
        return pnew;
    }
    template<typename T>
    inline unique_ptr<T> to_unique(unique_ptr<T> &&ptr) {
        auto p = ptr.get();
        ptr.release();
        auto pnew = unique_ptr<T>(p);
        return pnew;
    }

    template<typename T>
    inline shared_ptr<T> to_shared(T *ptr) { return shared_ptr<T>(ptr); }
    template<typename T>
    inline shared_ptr<T> to_shared(shared_ptr<T> const &ptr) { return ptr; }
    template<typename T>
    inline shared_ptr<T> to_shared(unique_ptr<T> &&ptr) { return ptr; }

} // namespace std

// ------------------- arg-count, mem-arg-count, ...
namespace cmdr::traits {

    // https://stackoverflow.com/questions/36797770/get-function-parameters-count


    /**
     * @brief 
     * @tparam R 
     * @tparam Types 
     * @param f 
     * @return 
     * @details For example:
     * @code{c++}
     * void foo2(int, int, char*) { }
     * size_t count = cmdr::traits::args_count(foo2);
     * @endcode
     */
    template<typename R, typename... Types>
    constexpr size_t args_count(R (*f)(Types...)) {
        UNUSED(f);
        return sizeof...(Types);
    }

    /**
     * @brief 
     * @tparam R 
     * @tparam T 
     * @tparam Types 
     * @return 
     * @details For example:
     * @code{c++}
     * struct s {
     *   void m(char *, int &) { std::cout << "member function\n"; }
     * };
     * const int c3 = cmdr::traits::member_args_count(&s::m).value;
     */
    template<typename R, typename T, typename... Types>
    constexpr std::integral_constant<unsigned, sizeof...(Types)>
    member_args_count(R (T::*)(Types...)) {
        return std::integral_constant<unsigned, sizeof...(Types)>{};
    }


    /**
     * @brief 
     * @tparam R 
     * @tparam Args 
     * @return 
     * @details For example:
     * @code{c++}
     * inline void foo1(int a, int b, int c){ UNUSED(a, b, c); }
     * size_t count = cmdr::traits::argCounter(foo1);
     * @endcode
     */
    template<class R, class... Args>
    constexpr auto argCounter(R(Args...)) {
        return sizeof...(Args);
    }

    /**
     * @brief 
     * @tparam function
     * @details For example:
     * @code{c++}
     * inline void foo1(int, int, int){}
     * size_t count = cmdr::traits::argCount<foo1>;
     * @endcode
     */
    template<auto function>
    inline constexpr auto argCount = argCounter(function);


    template<class R, class... ARGS>
    struct function_ripper {
        static constexpr size_t n_args = sizeof...(ARGS);
    };

    /**
     * @brief 
     * @tparam R 
     * @tparam ARGS 
     * @return 
     * @details For example:
     * @code{c++}
     * void foo(int, double, const char*);
     * void check_args() {
     *   constexpr size_t foo_args = decltype(cmdr::traits::make_ripper(foo))::n_args;
     *   std::cout &lt;&lt;"Foo has  " &lt;&lt; foo_args &lt;&lt; " arguments.\n";
     * }
     * @endcode
     */
    template<class R, class... ARGS>
    auto constexpr make_ripper(R(ARGS...)) {
        return function_ripper<R, ARGS...>();
    }

    /**
     * @brief 
     * @tparam func
     * @details For example:
     * @code{c++}
     * void foo(int, double, const char*);
     * void check_args() {
     *   constexpr size_t foo_args = decltype(cmdr::traits::make_ripper(foo))::n_args;
     *   std::cout &lt;&lt; "Foo has  " &lt;&lt; foo_args &lt;&lt; " arguments.\n";
     * }
     * @endcode
     */
    template<auto func>
    constexpr size_t n_args = decltype(make_ripper(func))::n_args;


    // -------------------

    template<typename Function>
    struct function_traits : public function_traits<decltype(&Function::operator())> {};

    template<typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType (ClassType::*)(Args...) const> {
        typedef ReturnType (*pointer)(Args...);
        typedef const std::function<ReturnType(Args...)> function;
    };

    template<typename Function>
    inline typename function_traits<Function>::function to_function(Function &lambda) {
        return static_cast<typename function_traits<Function>::function>(lambda);
    }

    template<class L>
    struct overload_lambda : L {
        overload_lambda(L l)
            : L(l) {}
        template<typename... T>
        void operator()(T &&...values) {
            // here you can access the target std::function with
            to_function (*(L *) this)(std::forward<T>(values)...);
        }
    };

    /**
     * @brief convert a lambda to std::function
     * @tparam L 
     * @param l 
     * @return std::function<R(Args...)>
     * @details For example:
     * @code{c++}
     *     auto fn = cmdr::types::lambda([&](std::vector<std::string> const &vec){...});
     *     fn(vec);
     * @endcode
     */
    template<class L>
    inline overload_lambda<L> lambda(L l) {
        return overload_lambda<L>(l);
    }


    // -------------------

    template<typename T>
    struct memfun_type {
        using type = void;
    };

    template<typename Ret, typename Class, typename... Args>
    struct memfun_type<Ret (Class::*)(Args...) const> {
        using type = std::function<Ret(Args...)>;
    };

    /**
     * @brief wrap a lambda as a std::function<...>
     * @tparam F 
     * @param func 
     * @return
     * @see https://stackoverflow.com/a/24068396/6375060
     */
    template<typename F>
    typename memfun_type<decltype(&F::operator())>::type inline lambda_to_function(F const &func) { return func; }
    template<typename F>
    typename memfun_type<decltype(&F::operator())>::type inline l2f(F const &func) { return func; }

} // namespace cmdr::traits

// ------------------- iterate, first_of, typelist, tag
namespace cmdr::traits {

    // ---------------------------

    template<std::size_t N, class = std::make_index_sequence<N>>
    struct iterate;

    template<std::size_t N, std::size_t... Is>
    struct iterate<N, std::index_sequence<Is...>> {
        template<class Lambda>
        auto operator()(Lambda lambda) {
            return lambda(std::integral_constant<std::size_t, Is>{}...);
        }
    };

    template<size_t... Is>
    struct first_of_A {};

    template<size_t N, size_t... Is>
    auto first_of() {
        return iterate<N>{}([](auto... ps) {
            using type = std::tuple<std::integral_constant<std::size_t, Is>...>;
            return first_of_A<std::tuple_element_t<ps, type>{}...>{};
        });
    }

    // ----------------------------

    template<class... Ts>
    struct typelist {
        using type = typelist;
        static constexpr std::size_t size = sizeof...(Ts);
    };

    template<class T>
    struct tag { using type = T; };

    namespace detail::another_head_n {
        template<std::size_t N, class R, class TL>
        struct head_n_impl;

        // have at least one to pop from and need at least one more, so just
        // move it over
        template<std::size_t N, class... Ts, class U, class... Us>
        struct head_n_impl<N, typelist<Ts...>, typelist<U, Us...>>
            : head_n_impl<N - 1, typelist<Ts..., U>, typelist<Us...>> {};

        // we have two base cases for 0 because we need to be more specialized
        // than the previous case regardless of if we have any elements in the list
        // left or not
        template<class... Ts, class... Us>
        struct head_n_impl<0, typelist<Ts...>, typelist<Us...>>
            : tag<typelist<Ts...>> {};

        template<class... Ts, class U, class... Us>
        struct head_n_impl<0, typelist<Ts...>, typelist<U, Us...>>
            : tag<typelist<Ts...>> {};

        template<std::size_t N, class TL>
        using head_n = typename head_n_impl<N, typelist<>, TL>::type;
    } // namespace detail::another_head_n

} // namespace cmdr::traits

// ------------------- head_n, drop_from_end
namespace cmdr::traits {

    template<typename... Pack>
    struct pack {};


    template<typename, typename>
    struct add_to_pack;

    template<typename A, typename... R>
    struct add_to_pack<A, pack<R...>> {
        typedef pack<A, R...> type;
    };


    template<typename>
    struct convert_to_tuple;

    template<typename... A>
    struct convert_to_tuple<pack<A...>> {
        typedef std::tuple<A...> type;
    };


    template<int, typename...>
    struct take;

    template<int N>
    struct take<N> {
        typedef pack<> type;
    };

    template<int N, typename Head, typename... Tail>
    struct take<N, Head, Tail...> {
        typedef
                typename std::conditional<
                        (N > 0),
                        typename add_to_pack<
                                Head,
                                typename take<
                                        N - 1,
                                        Tail...>::type>::type,
                        pack<>>::type type;
    };


    template<int, int, typename...>
    struct head;

    template<int I, int N>
    struct head<I, N> {
        typedef pack<> type;
    };

    template<int I, int N, typename Head, typename... Tail>
    struct head<I, N, Head, Tail...> {
        typedef
                typename std::conditional<
                        (N > I),
                        typename add_to_pack<
                                Head,
                                typename head<
                                        I + 1, N,
                                        Tail...>::type>::type,
                        pack<>>::type type;
    };

    /**
     * @brief drop the tailed Nth elements from variadic template arguments
     * @tparam N 
     * @tparam A 
     * @detail For example:
     * @code{c++}
     *   cmdr::traits::drop_from_end&lt;3, const char*, double, int, int&gt;::type b{"pi"};
     *   std::cout &lt;&lt; b &lt;&lt; '\n';
     *
     *   cmdr::traits::drop_from_end&lt;2, const char*, double, int, int&gt;::type c{"pi", 3.1415};
     *   detail::print_tuple(std::cout, c) &lt;&lt; '\n';
     * @endcode
     */
    template<int N, typename... A>
    struct drop_from_end {
        // Add these asserts if needed.
        //static_assert(N >= 0,
        //  "Cannot drop negative number of elements!");

        //static_assert(N <= static_cast<int>(sizeof...(A)),
        //  "Cannot drop more elements than size of pack!")

        typedef
                typename convert_to_tuple<
                        typename take<
                                static_cast<int>(sizeof...(A)) - N,
                                A...>::type>::type type;
    };

    /**
     * @brief collect/keep the head Nth elements from variadic template arguments
     * @tparam N 
     * @tparam A 
     * @detail For example:
     * @code{c++}
     *   cmdr::traits::head_n&lt;2, const char*, double, int, int&gt;::type hn{"pi", 3.1415};
     *   detail::print_tuple(std::cout, hn) &lt;&lt; '\n';
     *
     *   cmdr::traits::head_n&lt;3, const char*, double, int, int&gt;::type hn3{"pi", 3.1415, 1};
     *   std::cout &lt;&lt; hn3 &lt;&lt; '\n';
     * @endcode
     */
    template<int N, typename... A>
    struct head_n {
        typedef
                typename convert_to_tuple<
                        typename head<
                                0, N,
                                A...>::type>::type type;
    };

} // namespace cmdr::traits

// ------------------- cxx20 head/tail/slice
namespace cmdr::traits {
#if __cplusplus >= 202001

    namespace detail {
        template<typename T, auto Start, auto Step, T... Is>
        constexpr auto make_cons_helper_impl_(std::integer_sequence<T, Is...>) {
            auto eval_ = [](const T &I) consteval->T { return Start + Step * I; };
            return std::integer_sequence<T, eval_(Is)...>{};
        }

        template<typename T, auto Start, auto Count, auto Step>
        constexpr auto make_cons_impl_() {
            return make_cons_helper_impl_<T, Start, Step>(std::make_integer_sequence<T, Count>{});
        }
    } // namespace detail

    template<std::integral T, auto Start, auto Count, auto Step = 1>
    using make_consecutive_integer_sequence = decltype(detail::make_cons_impl_<T, Start, Count, Step>());

    template<auto Start, auto Count, auto Step = 1>
    using make_consecutive_index_sequence = make_consecutive_integer_sequence<std::size_t, Start, Count, Step>;

    template<std::size_t N>
    using make_first_n_index_sequence = make_consecutive_index_sequence<0, N>;

    template<std::size_t N, std::size_t S>
    using make_last_n_index_sequence = make_consecutive_index_sequence<S - N, N>;

    template<std::size_t B, std::size_t E>
    using make_slice_index_sequence = make_consecutive_index_sequence<B, E - B>;
    template<typename... Ts, std::size_t... Is>
    constexpr auto get_subpack_by_seq(std::index_sequence<Is...>, Ts &&...args) {
        return std::make_tuple(std::get<Is>(std::forward_as_tuple(args...))...);
    }

    template<std::size_t N, typename... Ts>
    requires(N <= sizeof...(Ts)) constexpr auto head(Ts &&...args) {
        return get_subpack_by_seq(
                make_first_n_index_sequence<N>{},
                std::forward<Ts>(args)...);
    }

    template<std::size_t N, typename... Ts>
    requires(N <= sizeof...(Ts)) constexpr auto tail(Ts &&...args) {
        return get_subpack_by_seq(
                make_last_n_index_sequence<N, sizeof...(Ts)>{},
                std::forward<Ts>(args)...);
    }

    template<std::size_t B, std::size_t E, typename... Ts>
    requires(B < E && B <= sizeof...(Ts) && E <= sizeof...(Ts)) constexpr auto slice(Ts &&...args) {
        return get_subpack_by_seq(
                make_slice_index_sequence<B, E>{},
                std::forward<Ts>(args)...);
    }

    static_assert(head<3>(1, 2.0f, "three", '4') == std::make_tuple(1, 2.0f, "three"));
    static_assert(tail<2>(1, 2.0f, "three", '4') == std::make_tuple("three", '4'));
    static_assert(slice<1, 3>(1, 2.0f, "three", '4') == std::make_tuple(2.0f, "three"));

#endif // C++20 or later
} // namespace cmdr::traits

// ------------------- light-weight bind
namespace cmdr::util {

    /**
     * @brief bind a lambda or function into std::function<...>
     * @tparam _Callable 
     * @tparam _Args 
     * @param f 
     * @param args 
     * @return 
     * @details For example:
     * @code{c++}
     *   struct moo {
     *     int doit(int x, int y) { return x + y; }
     *   };
     *   
     *   auto fn0 = cmdr::traits::bind([](int a, float b) {
     *     std::cout &lt;&lt; a &lt;&lt; ',' &lt;&lt; b &lt;&lt; '\n';
     *   }, _1, _2);
     *   fn0(1, 20.f);
     *
     *   moo m;
     *   auto fn1 = cmdr::traits::bind(&moo::doit, m, _1, 3.0f);
     *   std::cout &lt;&lt; '\n' &lt;&lt; fn1(1);
     *   
     *   auto fn2 = cmdr::traits::bind(doit, _1, 3.0f);
     *   std::cout &lt;&lt; '\n' &lt;&lt; fn2(9);
     * @endcode
     */
    template<typename _Callable, typename... _Args>
    inline auto bind(_Callable &&f, _Args &&...args) {
        auto fn = std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
        return fn;
    }

} // namespace cmdr::util

// ------------------- indices
namespace cmdr::traits {
    // @see http://loungecpp.wikidot.com/tips-and-tricks:indices

    template<std::size_t... Is>
    struct indices {};

    template<std::size_t N, std::size_t... Is>
    struct build_indices
        : build_indices<N - 1, N - 1, Is...> {};

    template<std::size_t... Is>
    struct build_indices<0, Is...> : indices<Is...> {};

    template<typename T>
    using Bare = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

    template<typename Tuple>
    using IndicesFor = build_indices<std::tuple_size<Bare<Tuple>>::value>;

    template<typename Tuple, std::size_t... Indices>
    std::array<int, std::tuple_size<Tuple>::value> f_them_all(Tuple &&t, indices<Indices...>) {
        return std::array<int, std::tuple_size<Tuple>::value>{{f(std::get<Indices>(std::forward<Tuple>(t)))...}};
    }
} // namespace cmdr::traits

namespace cmdr::traits {
    template<int I>
    struct placeholder {};
} // namespace cmdr::traits

namespace std {
    template<int I>
    struct is_placeholder<cmdr::traits::placeholder<I>> : std::integral_constant<int, I> {};
} // namespace std

// ------------------- easy_bind, bind_this
namespace cmdr::util {
    // ------------------- easy bind
    using namespace cmdr::traits;
    namespace detail {
        template<std::size_t... Is, class F, class... Args>
        inline auto easy_bind(indices<Is...>, F const &f, Args &&...args)
                -> decltype(std::bind(f, std::forward<Args>(args)..., placeholder<Is + 1>{}...)) {
            return std::bind(f, std::forward<Args>(args)..., placeholder<Is + 1>{}...);
        }
    } // namespace detail

    template<class R, class... FArgs, class... Args>
    inline auto easy_bind(std::function<R(FArgs...)> const &f, Args &&...args)
            -> decltype(detail::easy_bind(build_indices<sizeof...(FArgs) - sizeof...(Args)>{}, f, std::forward<Args>(args)...)) {
        return detail::easy_bind(build_indices<sizeof...(FArgs) - sizeof...(Args)>{}, f, std::forward<Args>(args)...);
    }

    // ------------------- bind_this
    template<class C, typename Ret, typename... Ts>
    inline std::function<Ret(Ts...)> bind_this(C *c, Ret (C::*m)(Ts...)) {
        return [=](auto &&...args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
    }

    template<class C, typename Ret, typename... Ts>
    inline std::function<Ret(Ts...)> bind_this(const C *c, Ret (C::*m)(Ts...) const) {
        return [=](auto &&...args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
    }
} // namespace cmdr::util

// ------------------- cool::bind_tie
namespace cmdr::util::cool {

    template<typename _Callable, typename... _Args>
    auto bind(_Callable &&f, _Args &&...args) {
        return std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
    }

    template<typename Function, typename Tuple, size_t... I>
    auto bind_N(Function &&f, Tuple &&t, std::index_sequence<I...>) {
        return std::bind(f, std::get<I>(t)...);
    }
    template<int N, typename Function, typename Tuple>
    auto bind_N(Function &&f, Tuple &&t) {
        // static constexpr auto size = std::tuple_size<Tuple>::value;
        return bind_N(f, t, std::make_index_sequence<N>{});
    }

    template<int N, typename _Callable, typename... _Args,
             std::enable_if_t<!std::is_member_function_pointer_v<_Callable>, bool> = true>
    auto bind_tie(_Callable &&f, _Args &&...args) {
        return bind_N<N>(f, std::make_tuple(args...));
    }

    template<typename Function, typename _Instance, typename Tuple, size_t... I>
    auto bind_N_mem(Function &&f, _Instance &&ii, Tuple &&t, std::index_sequence<I...>) {
        return std::bind(f, ii, std::get<I>(t)...);
    }
    template<int N, typename Function, typename _Instance, typename Tuple>
    auto bind_N_mem(Function &&f, _Instance &&ii, Tuple &&t) {
        return bind_N_mem(f, ii, t, std::make_index_sequence<N>{});
    }

    template<int N, typename _Callable, typename _Instance, typename... _Args,
             std::enable_if_t<std::is_member_function_pointer_v<_Callable>, bool> = true>
    auto bind_tie_mem(_Callable &&f, _Instance &&ii, _Args &&...args) {
        return bind_N_mem<N>(f, ii, std::make_tuple(args...));
    }
    template<int N, typename _Callable, typename... _Args,
             std::enable_if_t<std::is_member_function_pointer_v<_Callable>, bool> = true>
    auto bind_tie(_Callable &&f, _Args &&...args) {
        return bind_tie_mem<N>(std::forward<_Callable>(f), std::forward<_Args>(args)...);
    }

} // namespace cmdr::util::cool

// ------------------- get_template_type_t, return_type_of_t
namespace cmdr::traits {

    /**
     * @brief 
     * @tparam C
     * @code{c++}
     * template<class T>
     * class baz{
     * };
     * 
     * using ret = cmdr::traits::get_template_type_t<baz>::type;
     */
    template<typename C>
    struct get_template_type_t;
    template<template<typename> class C, typename T>
    struct get_template_type_t<C<T>> {
        using type = T;
    };

    /**
     * @brief 
     * @code{c++}
     * int foo(int a, int b, int c, int d) {
     *     return 1;
     * }
     * 
     * auto bar = [](){ return 1; };
     * 
     * struct baz_ 
     * { 
     *     double operator()(){ return 0; } 
     * } baz;
     * 
     * using ReturnTypeOfFoo = return_type_of_t<decltype(foo)>;
     * using ReturnTypeOfBar = return_type_of_t<decltype(bar)>;
     * using ReturnTypeOfBaz = return_type_of_t<decltype(baz)>;
     * @endcode
     */
    template<typename Callable>
    using return_type_of_t =
            typename decltype(std::function{std::declval<Callable>()})::result_type;

    /**
     * @brief 
     * @tparam R 
     * @tparam Args 
     * @return
     * @code{c++}
     * using ReturnTypeOfFoo = decltype(return_type_of(foo));
     * @endcode
     */
    template<typename R, typename... Args>
    R return_type_of(R (*)(Args...));


} // namespace cmdr::traits

// ------------------- singleton
namespace cmdr::util {

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
        static std::unique_ptr<T> instance{new T{token{}}};
        return *instance;
    }

    // template<typename T>
    // using hus = cmdr::util::singleton<T>;

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

} // namespace cmdr::util

#define CMDR_SINGLETON(t) cmdr::util::singleton<t>

// ------------------- defer
namespace cmdr::util {

    /**
     * @brief defer&lt;T&gt; provides a RAII wrapper for your lambda function.
     * @tparam T is a class which has a member function: <code>void close();</code>
     * @details For example:
     * @code{c++}
     *     std::ofstream ofs("aa.dot");
     *     cmdr::util::defer ofs_closer(ofs);
     *     cmdr::util::defer ofs_closer(ofs, []{ return; });
     *     
     *     // with a lambda
     *     cmdr::util::defer&lt;bool&gt; fn_closer([]{
     *         // ... your closer here
     *     });
     * @endcode
     * \br The alternate approach via std::unique_ptr:
     * @code{c++}
     *     auto ofs = std::make_unique&lt;std::ofstream&gt;("aa.dot", std::ofstream::out);
     *     *ofs.get() &lt;&lt; "1";
     * @endcode
     */
    template<class T = bool, class _D = std::default_delete<T>>
    class defer final {
    public:
        defer(T &c, _D const &fn = _D{})
            : _c(c)
            , _fn(fn) {}
        defer(_D const &fn, T &c = T{})
            : _c(c)
            , _fn(fn) {}
        ~defer() {
            _c.close();
            if (_fn) { _fn(); }
        }

    private:
        T &_c;
        _D _fn;
    };

    /**
     * @brief defer provides a RAII wrapper for your lambda function.
     * @details For example:
     * @code{c++}
     *     cmdr::util::defer&lt;bool&gtl fn_closer([]{
     *         // ... your closer here
     *     });
     * @endcode
     */
    template<>
    class defer<bool> final {
    public:
        defer(std::function<void()> const &fn, bool = false)
            : _fn(fn) {}
        ~defer() {
            if (_fn) { _fn(); }
        }

    private:
        std::function<void()> _fn;
    };

} // namespace cmdr::util

// ------------------- visitor
namespace cmdr::util {

    struct base_visitor {
        virtual ~base_visitor() {}
    };
    struct base_visitable {
        virtual ~base_visitable() {}
    };

    template<typename Visited, typename ReturnType = void>
    class visitor : public base_visitor {
    public:
        using return_t = ReturnType;
        using visited_t = std::unique_ptr<Visited>;
        virtual return_t visit(visited_t const &visited) = 0;
    };

    template<typename Visited, typename ReturnType = void>
    class visitable : public base_visitable {
    public:
        virtual ~visitable() {}
        using return_t = ReturnType;
        using visitor_t = visitor<Visited, return_t>;
        virtual return_t accept(visitor_t &guest) = 0;
    };

} // namespace cmdr::util

// ------------------- observer
namespace cmdr::util {

    template<typename S>
    class observer {
    public:
        virtual ~observer() {}
        using subject_t = S;
        virtual void observe(subject_t const &e) = 0;
    };

    /**
     * @brief an observable object which allows an observer registered.
     * @tparam S         subject or event
     * @tparam Observer 
     * @tparam AutoLock  thread-safe even if modifying observers chain dynamically
     * @tparam CNS       use Copy-and-Swap to shorten locking time.
     */
    template<typename S, bool AutoLock = true, bool CNS = true, typename Observer = observer<S>>
    class observable {
    public:
        virtual ~observable() { clear(); }
        using subject_t = S;
        using observer_t_nacked = Observer;
        using observer_t = std::weak_ptr<observer_t_nacked>;
        using observer_t_shared = std::shared_ptr<observer_t_nacked>;
        observable &add_observer(observer_t const &o) {
            if (AutoLock) {
                if (CNS) {
                    auto copy = _observers;
                    copy.push_back(o);
                    std::lock_guard _l(_m);
                    _observers.swap(copy);
                } else {
                    std::lock_guard _l(_m);
                    _observers.push_back(o);
                }
            } else
                _observers.push_back(o);
            return (*this);
        }
        observable &add_observer(observer_t_shared &o) {
            observer_t wp = o;
            if (AutoLock) {
                if (CNS) {
                    auto copy = _observers;
                    copy.push_back(wp);
                    std::lock_guard _l(_m);
                    _observers.swap(copy);
                } else {
                    std::lock_guard _l(_m);
                    _observers.push_back(wp);
                }
            } else
                _observers.push_back(wp);
            return (*this);
        }
        observable &remove_observer(observer_t_shared &o) { return remove_observer(o.get()); }
        observable &remove_observer(observer_t_nacked *o) {
            if (AutoLock) {
                if (CNS) {
                    auto copy = _observers;
                    copy.erase(std::remove_if(copy.begin(), copy.end(), [o](observer_t const &rhs) {
                                   if (auto spt = rhs.lock())
                                       return spt.get() == o;
                                   return false;
                               }),
                               copy.end());
                    std::lock_guard _l(_m);
                    _observers.swap(copy);
                } else {
                    std::lock_guard _l(_m);
                    _observers.erase(std::remove_if(_observers.begin(), _observers.end(), [o](observer_t const &rhs) {
                                         if (auto spt = rhs.lock())
                                             return spt.get() == o;
                                         return false;
                                     }),
                                     _observers.end());
                }
            } else
                _observers.erase(std::remove_if(_observers.begin(), _observers.end(), [o](observer_t const &rhs) {
                                     if (auto spt = rhs.lock())
                                         return spt.get() == o;
                                     return false;
                                 }),
                                 _observers.end());
            return (*this);
        }
        friend observable &operator+(observable &lhs, observer_t_shared &o) { return lhs.add_observer(o); }
        friend observable &operator+(observable &lhs, observer_t const &o) { return lhs.add_observer(o); }
        friend observable &operator-(observable &lhs, observer_t_shared &o) { return lhs.remove_observer(o); }
        friend observable &operator-(observable &lhs, observer_t_nacked *o) { return lhs.remove_observer(o); }
        observable &operator+=(observer_t_shared &o) { return add_observer(o); }
        observable &operator+=(observer_t const &o) { return add_observer(o); }
        observable &operator-=(observer_t_shared &o) { return remove_observer(o); }
        observable &operator-=(observer_t_nacked *o) { return remove_observer(o); }

    public:
        /**
         * @brief fire an event along the observers chain.
         * @param event_or_subject 
         */
        void emit(subject_t const &event_or_subject) {
            if (AutoLock) {
                std::lock_guard _l(_m);
                for (auto const &wp : _observers)
                    if (auto spt = wp.lock())
                        spt->observe(event_or_subject);
            } else {
                for (auto const &wp : _observers)
                    if (auto spt = wp.lock())
                        spt->observe(event_or_subject);
            }
        }

    private:
        void clear() {
            if (AutoLock) {
                std::lock_guard _l(_m);
                _observers.clear();
            }
        }

    private:
        std::vector<observer_t> _observers{};
        std::mutex _m{};
    };

    template<typename S, bool AutoLock = true, bool CNS = true, typename Observer = observer<S>>
    struct registerer {
        using _Observable = observable<S, AutoLock, CNS, Observer>;
        _Observable &_observable;
        typename _Observable::observer_t_shared &_observer;
        registerer(_Observable &observable, typename _Observable::observer_t_shared &observer)
            : _observable(observable)
            , _observer(observer) {
            _observable += _observer;
        }
        ~registerer() {
            _observable -= _observer;
        }
    };

    /**
     * @brief an observable object, which allows a lambda or a function to be bound as the observer.
     * @tparam S subject or event will be emitted to all bound observers.
     * 
     */
    template<typename S>
    class observable_bindable {
    public:
        virtual ~observable_bindable() { clear(); }
        using subject_t = S;
        using FN = std::function<void(subject_t const &)>;

        template<typename _Callable, typename... _Args>
        observable_bindable &add_callback(_Callable &&f, _Args &&...args) {
            FN fn = std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)..., std::placeholders::_1);
            _callbacks.push_back(fn);
            return (*this);
        }
        template<typename _Callable, typename... _Args>
        observable_bindable &on(_Callable &&f, _Args &&...args) { return add_callback(f, args...); }

        /**
         * @brief fire an event along the observers chain.
         * @param event_or_subject 
         */
        void emit(subject_t const &event_or_subject) {
            for (auto &fn : _callbacks)
                fn(event_or_subject);
        }

    private:
        void clear() {}

    private:
        std::vector<FN> _callbacks{};
    };

} // namespace cmdr::util

// ------------------- signal & slot
namespace cmdr::util {

    /**
     * @brief A covered pure C++ implementation for QT signal-slot mechanism
     * @tparam SignalSubjects 
     */
    template<typename... SignalSubjects>
    class signal {
    public:
        virtual ~signal() { clear(); }
        using FN = std::function<void(SignalSubjects &&...)>;
        static constexpr std::size_t SubjectCount = sizeof...(SignalSubjects);

        template<typename _Callable, typename... _Args>
        signal &connect(_Callable &&f, _Args &&...args) {
            using namespace std::placeholders;
            FN fn = cool::bind_tie<SubjectCount>(std::forward<_Callable>(f), std::forward<_Args>(args)..., _1, _2, _3, _4, _5, _6, _7, _8, _9);
            _callbacks.push_back(fn);
            return (*this);
        }
        template<typename _Callable, typename... _Args>
        signal &on(_Callable &&f, _Args &&...args) {
            using namespace std::placeholders;
            FN fn = util::cool::bind_tie<SubjectCount>(std::forward<_Callable>(f), std::forward<_Args>(args)..., _1, _2, _3, _4, _5, _6, _7, _8, _9);
            _callbacks.push_back(fn);
            return (*this);
        }

        /**
         * @brief fire an event along the observers chain.
         * @param event_or_subject 
         */
        signal &emit(SignalSubjects &&...event_or_subjects) {
            for (auto &fn : _callbacks)
                fn(std::move(event_or_subjects)...);
            return (*this);
        }
        signal &operator()(SignalSubjects &&...event_or_subjects) { return emit(event_or_subjects...); }

    private:
        void clear() {}

    private:
        std::vector<FN> _callbacks{};
    };

} // namespace cmdr::util

// ------------------- detect_shell_env
namespace cmdr::util {

    inline std::string detect_shell_env() {
        auto *str = std::getenv("SHELL");
        if (str != nullptr) {
            auto path = std::filesystem::path(str);
            return path.filename().u8string();
        }
        return "unknown";
    }

} // namespace cmdr::util

// ------------------- is_in
namespace cmdr::util {

    /**
     * @brief 
     * @tparam First 
     * @tparam T 
     * @param first 
     * @param t 
     * @return 
     * @code{c++}
     * if (is_in(s1, s2, s3, s4)) // ...
     * @endcode
     */
    template<typename First, typename... T>
    inline bool is_in(First &&first, T &&...t) {
        return ((first == t) || ...);
    }

} // namespace cmdr::util

namespace cmdr::util {
    template<class T>
    inline bool compare_vector_values(std::vector<T> const &v1, std::vector<T> const &v2) {
        bool not_ok = false;
        if (v1.size() == v2.size()) {
            for (std::size_t i = 0; i < v1.size(); i++) {
                if (std::strcmp(v1[i], v2[i]) != 0) {
                    not_ok = true;
                    break;
                }
            }
        } else
            not_ok = true;
        return (not_ok == false);
    }
} // namespace cmdr::util

// ------------------- safe_bool
namespace cmdr {
    class safe_bool_base {
    public:
        typedef void (safe_bool_base::*bool_type)() const;
        void this_type_does_not_support_comparisons() const {}

    protected:
        safe_bool_base() {}
        safe_bool_base(const safe_bool_base &) {}
        safe_bool_base &operator=(const safe_bool_base &) { return *this; }
        ~safe_bool_base() {}
    };

    // For testability without virtual function.
    /**
     * @brief 
     * @tparam T 
     * @note For C++11 or later, use explicit keyword, for instance:
     * @code{c++}
     * struct Testable {
     *   explicit operator bool() const {
     *     return false;
     *   }
     * };
     * @endcode
     * @details For example:
     * @code{c++}
     * #include <iostream>
     * 
     * class Testable_with_virtual : public safe_bool&lt;&gt; {
     * public:
     *     virtual ~Testable_with_virtual () {}
     * protected:
     *     virtual bool boolean_test() const {
     *         // Perform Boolean logic here
     *         return true;
     *     }
     * };
     * 
     * class Testable_without_virtual : 
     *     public safe_bool &lt;Testable_without_virtual&gt; // CRTP idiom
     * {
     * public:
     *     bool boolean_test() const { // NOT virtual
     *         // Perform Boolean logic here
     *         return false;
     *     }
     * };
     * 
     * int main (void) {
     *     Testable_with_virtual t1, t2;
     *     Testable_without_virtual p1, p2;
     *     if (t1) {}
     *     if (p1 == false) {
     *         std::cout &lt;&lt; "p1 == false\n";
     *     }
     *     if (p1 == p2) {} // Does not compile, as expected
     *     if (t1 != t2) {} // Does not compile, as expected
     * 
     *     return 0;
     * }
     * @endcode
     */
    template<typename T = void>
    class safe_bool : private safe_bool_base {
        // private or protected inheritance is very important here as it triggers the
        // access control violation in main.
    public:
        operator bool_type() const {
            return (static_cast<const T *>(this))->boolean_test()
                           ? &safe_bool_base::this_type_does_not_support_comparisons
                           : 0;
        }

    protected:
        ~safe_bool() {}
    };


    // For testability with a virtual function.
    template<>
    class safe_bool<void> : private safe_bool_base {
        // private or protected inheritance is very important here as it triggers the
        // access control violation in main.
    public:
        operator bool_type() const {
            return boolean_test()
                           ? &safe_bool_base::this_type_does_not_support_comparisons
                           : 0;
        }

    protected:
        virtual bool boolean_test() const = 0;
        virtual ~safe_bool() {}
    };

    template<typename T>
    bool operator==(const safe_bool<T> &lhs, bool b) {
        return b == static_cast<bool>(lhs);
    }

    template<typename T>
    bool operator==(bool b, const safe_bool<T> &rhs) {
        return b == static_cast<bool>(rhs);
    }


    template<typename T, typename U>
    bool operator==(const safe_bool<T> &lhs, const safe_bool<U> &) {
        lhs.this_type_does_not_support_comparisons();
        return false;
    }

    template<typename T, typename U>
    bool operator!=(const safe_bool<T> &lhs, const safe_bool<U> &) {
        lhs.this_type_does_not_support_comparisons();
        return false;
    }
} // namespace cmdr

// ------------------- heap_only, no_heap
namespace cmdr {

    template<typename T>
    class heap_only {
    public:
        heap_only() {}

        void destroy() const { delete this; }
        static T *create() { return new T{}; }

    protected:
        ~heap_only() {}
    };

    class VO : public heap_only<VO> {
    public:
        ~VO() {}
    };

    class no_heap {
    protected:
        static void *operator new(std::size_t);   // #1: To prevent allocation of scalar objects
        static void *operator new[](std::size_t); // #2: To prevent allocation of array of objects
    };

} // namespace cmdr

// ------------------- non_copyable
namespace cmdr {

    /**
     * @brief To prevent objects of a class from being copy-constructed or assigned to each other.
     * @code{c++}
     * class cant_copy : private non_copyable<cant_copy> {};
     * @endcode
     * @see macro CLAZZ_NON_COPYABLE(clz) and CLAZZ_NON_MOVEABLE(clz)
     */
    template<class T>
    class non_copyable {
    public:
        non_copyable(const non_copyable &) = delete;
        // non_copyable &operator=(const non_copyable &) = delete;
        T &operator=(const T &) = delete;

    protected:
        non_copyable() = default;
        ~non_copyable() = default; /// Protected non-virtual destructor
    };

    /**
     * @brief To prevent RVO or pass-and-copy value as function parameter
     * @code{c++}
     * no_implicit_copy foo() { // Compiler error because copy-constructor must be invoked implicitly to return by value.
     *     no_implicit_copy n;
     *     return n;
     * }
     * 
     * void bar(no_implicit_copy n) { // Compiler error because copy-constructor must be invoked implicitly to pass by value.
     * }
     * @endcode
     */
    struct no_implicit_copy {
        no_implicit_copy() = default;
        explicit no_implicit_copy(const no_implicit_copy &) = default;
    };

} // namespace cmdr


#endif //CMDR_CXX_CMDR_COMMON_HH
