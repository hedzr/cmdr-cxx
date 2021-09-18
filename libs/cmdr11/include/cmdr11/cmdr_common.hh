//
// Created by Hedzr Yeh on 2021/8/31.
//

#ifndef CMDR_CXX_CMDR_COMMON_HH
#define CMDR_CXX_CMDR_COMMON_HH

#include "cmdr_defs.hh"

#include <functional>
#include <memory>

namespace std {

    template<typename T>
    inline unique_ptr<T> to_unique(T *ptr) {
        return unique_ptr<T>(ptr);
    }
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
    inline shared_ptr<T> to_shared(T *ptr) {
        return shared_ptr<T>(ptr);
    }
    template<typename T>
    inline shared_ptr<T> to_shared(shared_ptr<T> const &ptr) {
        return ptr;
    }
    template<typename T>
    inline shared_ptr<T> to_shared(unique_ptr<T> &&ptr) {
        return ptr;
    }

} // namespace std

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

} // namespace cmdr::traits

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

namespace cmdr::util {

    template<typename S>
    class observer {
    public:
        virtual ~observer() {}
        using subject_t = S;
        virtual void observe(subject_t const &e) = 0;
    };

    /**
     * @brief 
     * @tparam S 
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
        std::vector<observer_t> _observers;
        std::mutex _m;
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

} // namespace cmdr::util

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
