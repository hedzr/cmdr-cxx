//
// Created by Hedzr Yeh on 2021/1/13.
//

#ifndef CMDR_CXX11_CMDR_UTILS_HH
#define CMDR_CXX11_CMDR_UTILS_HH

#include <any>
#include <optional>
#include <variant>

#include <list>
#include <vector>

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

#ifdef __cpp_lib_filesystem
#include <filesystem>
#else
#include <experimental/filesystem>
namespace std {
    namespace filesystem = experimental::filesystem;
}
#endif

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

    class defer final {
    public:
        defer(std::function<void()> const &fn)
            : _fn(fn) {}
        ~defer() {
            if (_fn) {
                _fn();
            }
        }

    private:
        std::function<void()> _fn;
    };


    inline std::string detect_shell_env() {
        auto *str = std::getenv("SHELL");
        if (str != nullptr) {
            auto path = std::filesystem::path(str);
            return path.filename().u8string();
        }
        return "unknown";
    }
    
} //namespace cmdr::util



namespace cmdr::util {

    template<class T, class PT = int>
    using comparer = std::function<PT(T const &lhs, T const &rhs)>;

    template<class T,
            class PT = int,
            class Comp = comparer<T, PT>,
            class Container = std::list<T>,
            bool ReverseComp = false>
    class priority_queue {
    public:
        struct element;
        using value_type = T;
        struct _It {
            element *_el{};
            std::size_t _pos{(std::size_t) -1};

            priority_queue::value_type *get() const;
            bool operator==(_It const &rhs) const { return _el == rhs._el && _pos == rhs._pos; }
            bool operator!=(_It const &rhs) const { return _el != rhs._el || _pos != rhs._pos; }
            bool operator()() const { return _el != nullptr; }
            bool operator!() const { return _el == nullptr; }
            element *operator->() { return _el; }

            _It() = default;
            _It(element *el, std::size_t pos)
                    : _el(el)
                      , _pos(pos) {}
            _It(const _It &o)
                    : _el(o._el)
                      , _pos(o._pos) {}
            _It &operator=(const _It &o) {
                _el = o._el;
                _pos = o._pos;
                return (*this);
            }
        };
        struct element {
            Container _list;
            int _min_value;
            std::shared_ptr<element> _left{};
            std::shared_ptr<element> _right{};
            element() = default;
            ~element() = default;
            element(Container &&l, int m)
                    : _list(l)
                      , _min_value(m) {}
            element(value_type l, int m)
                    : _list{}
                      , _min_value(m) {
                _list.push_back(std::move(l));
            }
            // pre-order traversal
            void NLR(std::function<void(element *)> const &fn) {
                fn(this);
                if (_left)
                    _left->LNR(fn);
                if (_right)
                    _right->LNR(fn);
            }
            // in-order traversal
            void LNR(std::function<void(element *)> const &fn) {
                if (_left)
                    _left->LNR(fn);
                fn(this);
                if (_right)
                    _right->LNR(fn);
            }
            // post-order traversal
            void LRN(std::function<void(element *)> const &fn) {
                if (_left)
                    _left->LRN(fn);
                if (_right)
                    _right->LRN(fn);
                fn(this);
            }
            // reverse in-order
            void RNL(std::function<void(element *)> const &fn) {
                if (_right)
                    _right->LRN(fn);
                fn(this);
                if (_left)
                    _left->LRN(fn);
            }

            T &pop(std::size_t &count) {
                if (ReverseComp) {
                    if (_left) {
                        std::size_t before = count;
                        T &t = _left->pop(count);
                        if (before > count)
                            return t;
                    }
                    if (!_list.empty()) {
                        count--;
                        T &t = _list.front();
                        _list.pop_front();
                        return t;
                    }
                    if (_right) {
                        std::size_t before = count;
                        T &t = _right->pop(count);
                        if (before > count)
                            return t;
                    }
                    return _null;
                }

                // normal
                if (_right) {
                    std::size_t before = count;
                    T &t = _right->pop(count);
                    if (before > count)
                        return t;
                }
                if (!_list.empty()) {
                    count--;
                    T &t = _list.front();
                    _list.pop_front();
                    return t;
                }
                if (_left) {
                    std::size_t before = count;
                    T &t = _left->pop(count);
                    if (before > count)
                        return t;
                }
                return _null;
            }

            _It next(std::size_t pos) {
                if (ReverseComp) {
                    if (pos == (std::size_t) -1) {
                        if (_left) {
                            auto p = _left->next(pos);
                            if (p._el != nullptr)
                                return p;
                        }
                        if (!_list.empty()) {
                            return {this, 0};
                        }
                        if (_right) {
                            auto p = _right->next(pos);
                            if (p._el != nullptr)
                                return p;
                        }
                    } else {
                        assert(!_list.empty() && _list.size() >= pos);
                        if (pos < _list.size() - 1)
                            return {this, pos + 1};
                        if (_right) {
                            auto p = _right->next(0);
                            if (p._el != nullptr)
                                return p;
                        }
                    }
                    return _It{};
                }

                // normal
                if (pos == (std::size_t) -1) {
                    if (_right) {
                        auto p = _right->next(pos);
                        if (p._el != nullptr)
                            return p;
                    }
                    if (!_list.empty()) {
                        return {this, 0};
                    }
                    if (_left) {
                        auto p = _left->next(pos);
                        if (p._el != nullptr)
                            return p;
                    }
                } else {
                    assert(!_list.empty() && _list.size() >= pos);
                    if (pos < _list.size() - 1)
                        return {this, pos + 1};
                    if (_left) {
                        auto p = _left->next(0);
                        if (p._el != nullptr)
                            return p;
                    }
                }
                return _It{};
            }

            static priority_queue::value_type _null;
        };
        struct my_iterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = priority_queue::value_type;
            using pointer = priority_queue::value_type *;
            using reference = priority_queue::value_type &;

            my_iterator() = default;
            my_iterator(element *ptr, std::size_t pos = (std::size_t) -1)
                    : _it{ptr, pos} {}

            reference operator*() const {
                _It &it = const_cast<_It &>(_it);
                // pointer ptr = it.get();
                auto &z = it._el->_list;
                auto x = z.begin();
                std::advance(x, it._pos);
                reference data = (*x);
                return data;
            }
            pointer operator->() {
                _It &it = const_cast<_It &>(_it);
                // return it.get();
                auto &z = it._el->_list;
                auto x = z.begin();
                std::advance(x, it._pos);
                reference data = (*x);
                auto *ptr = &data;
                return ptr;
            }
            my_iterator &operator++() {
                if (_it._el != nullptr)
                    _it = _it->next(_it._pos);
                return *this;
            }
            my_iterator operator++(int) {
                my_iterator tmp = *this;
                ++(*this);
                return tmp;
            }
            friend bool operator==(const my_iterator &a, const my_iterator &b) { return a._it == b._it; };
            friend bool operator!=(const my_iterator &a, const my_iterator &b) { return a._it != b._it; };

        private:
            _It _it;
        };

    public:
        priority_queue()
                : _root{std::make_shared<element>()}
                  , _comparer{} {}
        virtual ~priority_queue() {}

        my_iterator begin() { return my_iterator{_root.get(), 0}; }
        my_iterator end() { return my_iterator(nullptr); }
        void push_back(T const &data) { push(data); }
        void pop_front() { _root->pop(_count); }
        T &front() { return *begin(); }

        void push(T data) {
            _push(_root, std::move(data));
            _count++;
        }
        T &pop() { return _root->pop(_count); }
        bool empty() const { return _count == 0; }
        static bool is_null(T const &t) { return t == element::_null; }

    public:
        void dump(std::function<void(element *)> const &fn) { ReverseComp ? _root->LNR(fn) : _root->RNL(fn); }

    protected:
        void mid_walk(std::function<void(element *)> const &fn) { _root->LNR(fn); }
        void pot_walk(std::function<void(element *)> const &fn) { _root->rgt_walk(fn); }
        void lft_walk(std::function<void(element *)> const &fn) { _root->LRN(fn); }

    private:
        void _push(std::shared_ptr<element> &at, T data) {
            PT l{}, r{};
            if (!at->_list.empty()) {
                for (auto const &v : at->_list) {
                    PT ret = _comparer(data, v);
                    if (ret < 0) {
                        if (ret < l)
                            l = ret;
                    } else if (ret > 0) {
                        if (ret > r)
                            r = ret;
                    }
                }
            }

            if (l < 0) {
                if (at->_left) {
                    _push(at->_left, data);
                } else {
                    at->_left = std::make_shared<element>(element{data, l});
                }
                return;
            }

            if (r > 0) {
                if (at->_right) {
                    _push(at->_right, data);
                } else {
                    at->_right = std::make_shared<element>(element{data, r});
                }
                return;
            }

            at->_list.push_back(data);
        }

    private:
        std::shared_ptr<element> _root;
        Comp _comparer{};
        std::size_t _count{};
    };

    template<class T,
            class PT,
            class Comp,
            class Container,
            bool ReverseComp>
    inline typename priority_queue<T, PT, Comp, Container, ReverseComp>::value_type *
    priority_queue<T, PT, Comp, Container, ReverseComp>::_It::get() const {
        auto &z = const_cast<element *>(_el)->_list;
        auto x = z.begin();
        std::advance(x, _pos);
        auto &data = (*x);
        auto *ptr = &data;
        return ptr;
    };

    template<class T,
            class PT,
            class Comp,
            class Container,
            bool ReverseComp>
    inline typename priority_queue<T, PT, Comp, Container, ReverseComp>::value_type priority_queue<T, PT, Comp, Container, ReverseComp>::element::_null{};
    
}


#endif //CMDR_CXX11_CMDR_UTILS_HH
