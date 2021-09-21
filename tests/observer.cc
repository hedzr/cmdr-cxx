//
// Created by Hedzr Yeh on 2021/9/21.
//

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
#include <iomanip>
#include <iostream>
#include <string>

#include <math.h>

#include "cmdr11/cmdr_defs.hh"
#include "cmdr11/cmdr_utils.hh"
#include "cmdr11/cmdr_var_t.hh"

#include "cmdr11/cmdr_x_test.hh"


namespace cmdr::dp::observer::basic {

    struct event {
        // bool operator==(const event &) const { return true; }
        // bool operator==(const std::string_view &tn) const { return cmdr::debug::type_name<event>() == tn; }
    };

    struct mouse_move_event : public event {};

    class Store : public cmdr::util::observable<event, true> {};

    class Customer : public cmdr::util::observer<event> {
    public:
        virtual ~Customer() {}
        bool operator==(const Customer &r) const { return this == &r; }
        void observe(const subject_t &) override {
            cmdr_debug("event raised: %s", cmdr::debug::type_name<subject_t>().data());
        }
    };

} // namespace cmdr::dp::observer::basic

void test_observer_basic() {
    using namespace cmdr::dp::observer::basic;

    Store store;
    Store::observer_t_shared c = std::make_shared<Customer>(); // uses Store::observer_t_shared rather than 'auto'

    store += c;
    store.emit(event{});
    store -= c;

    cmdr::util::registerer<event, true> __r(store, c);
    store.emit(mouse_move_event{});
}

namespace helpers {

    template<typename... Args>
    inline void Callback(std::function<void(Args...)> f) {
        // store f and call later
        UNUSED(f);
    }

} // namespace helpers

struct moo {
    int doit(int x, int y) { return x + y; }
    bool isBetween(int i, int min, int max) { return i >= min && i <= max; }
};

int doit(int x, int y) { return x + y; }
bool isBetween(int i, int min, int max) { return i >= min && i <= max; }
void printNumber(std::vector<int> &number, std::function<bool(int)> filter) {
    for (const int &i : number) {
        if (filter(i)) {
            std::cout << i << ',';
        }
    }
}

void test_util_bind() {
    using namespace std::placeholders;
    {
        // std::bind

        std::vector<int> numbers{1, 2, 3, 4, 5, 10, 15, 20, 25, 35, 45, 50};
        std::function<bool(int)> filter = std::bind(isBetween, _1, 20, 40);
        printNumber(numbers, filter);
        filter = std::bind([](int i, int min, int max) { return i >= min && i <= max; }, _1, 20, 40);
        printNumber(numbers, filter);
        // filter = bind([](int i, int min, int max) { return i >= min && i <= max; }, _1, 20, 40);
        // printNumber(numbers, filter);

        // lambda_to_function

        helpers::Callback(cmdr::traits::l2f([](int a, float b) { std::cout << a << ',' << b << '\n'; }));

        // cmdr::util::bind

        auto fn0 = cmdr::util::bind([](int a, float b) { std::cout << "\nfn0: " << a << ',' << b << '\n'; }, _1, _2);
        fn0(1, 20.f);

#ifndef _MSC_VER
        moo m;
        auto fn1 = cmdr::util::bind(&moo::doit, m, _1, 3.0f);
        std::cout << "fn1: " << fn1(1) << '\n';

        auto fn2 = cmdr::util::bind(doit, _1, 3.0f);
        std::cout << "fn2: " << fn2(9) << '\n';
#else
        // in msvc, C4244 warning will be thrown since it's converting
        // float(3.0f) to int (the 2nd arg of doit()), the precision
        // will be lost in narrowing a number.
        moo m;
        auto fn1 = cmdr::util::bind(&moo::doit, m, _1, 3);
        std::cout << "fn1: " << fn1(1) << '\n';

        auto fn2 = cmdr::util::bind(doit, _1, 3);
        std::cout << "fn2: " << fn2(9) << '\n';
#endif
    }

    {
        auto fn = cmdr::traits::lambda([&](std::vector<std::string> const &vec) {
            std::cout << vec << '\n';
        });
        std::vector<std::string> vec{"aa", "bb", "cc"};
        std::function<void(std::vector<std::string> const &)> x = fn;
        x(vec);
        cmdr_print("lambda fn: %s", std::string(cmdr::debug::type_name<decltype(x)>()).c_str());
    }
}

namespace cmdr::dp::observer::cb {

    struct event {
        std::string to_string() const { return "event"; }
        // bool operator==(const event &) const { return true; }
        // bool operator==(const std::string_view &tn) const { return cmdr::debug::type_name<event>() == tn; }
    };

    struct mouse_move_event : public event {};

    class Store : public cmdr::util::observable_bindable<event> {};

} // namespace cmdr::dp::observer::cb

void fntest(cmdr::dp::observer::cb::event const &e) {
    cmdr_print("event CB regular function: %s", e.to_string().c_str());
}

void test_observer_cb() {
    using namespace cmdr::dp::observer::cb;
    using namespace std::placeholders;

    Store store;

    auto fn = [](event const &e) {
        cmdr_print("event: %s", e.to_string().c_str());
    };
    cmdr_print("lambda: %s", std::string(cmdr::debug::type_name<decltype(fn)>()).c_str());

    store.add_callback([](event const &e) {
        cmdr_print("event CB lamdba: %s", e.to_string().c_str());
    });
    struct eh1 {
        void cb(event const &e) {
            cmdr_print("event CB member fn: %s", e.to_string().c_str());
        }
        void operator()(event const &e) {
            cmdr_print("event CB member operator() fn: %s", e.to_string().c_str());
        }
    };
    store.on(&eh1::cb, eh1{});
    store.on(&eh1::operator(), eh1{});

    store.on(fntest);

    store.emit(mouse_move_event{});
}

namespace cmdr::dp::observer::slots {

    void f() { std::cout << "free function\n"; }

    struct s {
        void m(char *, int &) { std::cout << "member function\n"; }
        static void sm(char *, int &) { std::cout << "static member function\n"; }
        void ma() { std::cout << "member function\n"; }
        static void sma() { std::cout << "static member function\n"; }
    };

    struct o {
        void operator()() { std::cout << "function object\n"; }
    };

    inline void foo1(int, int, int) {}
    void foo2(int, int &, char *) {}

    struct example {
        template<typename... Args, typename T = std::common_type_t<Args...>>
        static std::vector<T> foo(Args &&...args) {
            std::initializer_list<T> li{std::forward<Args>(args)...};
            std::vector<T> res{li};
            return res;
        }
    };

} // namespace cmdr::dp::observer::slots

void test_observer_slots() {
    using namespace cmdr::dp::observer::slots;
    using namespace std::placeholders;
    {
        std::vector<int> v1 = example::foo(1, 2, 3, 4);
        for (const auto &elem : v1)
            std::cout << elem << " ";
        std::cout << "\n";

        // auto v2 = example::foo(1, 2.0, true, "str");
        // for (const auto &elem : v2)
        //     std::cout << elem << " ";
        // std::cout << "\n";
    }
    s d;
    auto lambda = []() { std::cout << "lambda\n"; };
    auto gen_lambda = [](auto &&...a) { std::cout << "generic lambda: "; (std::cout << ... << a) << '\n'; };
    UNUSED(d);

    cmdr::util::signal<> sig;

    sig.on(f);
    sig.connect(&s::ma, d);
    sig.on(&s::sma);
    sig.on(o());
    sig.on(lambda);
    sig.on(gen_lambda);

    sig(); // emit a signal
}

void test_observer_slots_args() {
    using namespace cmdr::dp::observer::slots;
    using namespace std::placeholders;

    struct foo {
        // Notice how we accept a double as first argument here.
        // This is fine because float is convertible to double.
        // 's' is a reference and can thus be modified.
        void bar(double d, int i, bool b, std::string &&s) {
            std::cout << "memfn: " << s << (b ? std::to_string(i) : std::to_string(d)) << '\n';
        }
    };

    // Function objects can cope with default arguments and overloading.
    // It does not work with static and member functions.
    struct obj {
        void operator()(float, int, bool, std::string &&, int = 0) {
            std::cout << "obj.operator(): I was here" << '\n';
        }

        // void operator()() {}
    };

    // a generic lambda that prints its arguments to stdout
    auto printer = [](auto a, auto &&...args) {
        std::cout << a;
        (void) std::initializer_list<int>{
                ((void) (std::cout << " " << args), 1)...};
        std::cout << '\n';
    };

    // declare a signal with float, int, bool and string& arguments
    cmdr::util::signal<float, int, bool, std::string> sig;

    // connect the slots
    sig.connect(printer, _1, _2, _3, _4);
    foo ff;
    sig.on(&foo::bar, ff, _1, _2, _3, _4);
    // sig.on(&foo::bar, ff);
    sig.on(obj(), _1, _2, _3, _4);

    float f = 1.f;
#ifdef _MSC_VER
    int i = 2;
#else
    short i = 2; // convertible to int
#endif
    std::string s = "0";

    // emit a signal
#ifdef _MSC_VER
    sig.emit(std::move(f), std::move(i), false, std::move(s));
    sig.emit(std::move(f), std::move(i), true, std::move(s));
#else
    sig.emit(std::move(f), i, false, std::move(s));
    sig.emit(std::move(f), i, true, std::move(s));
#endif
}

int main() {

    CMDR_TEST_FOR(test_util_bind);

    CMDR_TEST_FOR(test_observer_basic);
    CMDR_TEST_FOR(test_observer_cb);
    CMDR_TEST_FOR(test_observer_slots);
    CMDR_TEST_FOR(test_observer_slots_args);

    return 0;
}