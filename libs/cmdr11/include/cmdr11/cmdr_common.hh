//
// Created by Hedzr Yeh on 2021/8/31.
//

#ifndef CMDR_CXX_CMDR_COMMON_HH
#define CMDR_CXX_CMDR_COMMON_HH

#include "cmdr_defs.hh"

namespace cmdr::util {

    /**
     * @brief 
     * @tparam C
     * @code{c++}
     * template<class T>
     * class baz{
     * };
     * 
     * using ret = cmdr::util::get_template_type_t<baz>::type;
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


} // namespace cmdr::util

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

} // namespace cmdr::util

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

namespace cmdr::util {

    inline std::string detect_shell_env() {
        auto *str = std::getenv("SHELL");
        if (str != nullptr) {
            auto path = std::filesystem::path(str);
            return path.filename().u8string();
        }
        return "unknown";
    }

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

} //namespace cmdr::util

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

#endif //CMDR_CXX_CMDR_COMMON_HH
