//
// Created by Hedzr Yeh on 2021/1/17.
//

#include <any>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <new>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>


#include "cmdr11/cmdr_utils.hh"
#include "cmdr11/cmdr_var_t.hh"


class X {
public:
    X() = default;
    ~X() = default;
    X(X &&o)
    noexcept = delete;
    X &operator=(const X &o) = delete;

    template<typename A, typename... Args,
             std::enable_if_t<
                     std::is_constructible<std::any, A, Args...>::value &&
                             !std::is_same<std::decay_t<A>, X>::value,
                     int> = 0>
    explicit X(A &&a0, Args &&...args)
        : _arg(std::forward<A>(a0), std::forward<Args>(args)...) {
        std::cout << ".1." << a0 << std::endl;
    }
    template<typename A,
             std::enable_if_t<!std::is_same<std::decay_t<A>, std::any>::value &&
                                      !std::is_same<std::decay_t<A>, X>::value,
                              int> = 0>
    explicit X(A &&v)
        : _arg(std::forward<A>(v)) {
        std::cout << ".2." << v << std::endl;
    }

    friend std::ostream &operator<<(std::ostream &os, X const &a) {
        (void) a; // std::cout << a._arg;
        return os;
    }

    std::any _arg;
};

void test_streamer_any() {
    using cmdr::opt::vars::streamable_any;
    using cmdr::util::streamer_any;
    using namespace std::chrono_literals;

    std::cout << "test_streamable_any manually:" << '\n';

    streamable_any vo1{1};
    streamable_any vo2{true};
    streamable_any vo3{1.3};
    streamable_any vo4{(int32_t) -7};
    streamable_any vo5{"a string"};

    std::cout << vo1 << std::endl;
    std::cout << vo2 << std::endl;
    std::cout << vo3 << std::endl;
    std::cout << vo4 << std::endl;
    std::cout << vo5 << std::endl;

    std::cout << '\n';
}

void test_streamable_any() {
    using cmdr::opt::vars::streamable_any;
    using cmdr::util::streamer_any;
    using namespace std::chrono_literals;

    streamer_any os;
    std::vector<std::any> va{
            {},
            42,
            123u,
            3.14159f,
            2.71828,
            "C++17",
            501ns,
            "ns",
            7303us,
            250ms,
            "ms",
            0.5min,
            3min,
            7s,
            1024h,
            // 80y,
    };

    std::cout << "test_streamable_any:" << '\n';

    for (auto &v : va) {
        // std::cout << v << std::endl;
        std::cout << ".1.";
        os.process(std::cout, v);
        std::cout << '\n';
    }


    using namespace std::chrono;
    using namespace cmdr::chrono;

    // std::any a1(501ns);
    // std::cout << ".2.";
    // format_duration(std::cout, std::any_cast<nanoseconds>(a1));
    // std::cout << '\n';
    //
    // std::any a2(0.5min);
    // std::cout << ".2.";
    // format_duration(std::cout, std::any_cast<minutes>(a2));
    // std::cout << '\n';


    std::cout << '\n';
}

struct A {
    virtual ~A() {}
};

struct B : A {};
struct C : A {};

void test_types() {
    using namespace std::chrono_literals;
    std::unordered_map<std::type_index, std::string> type_names;

    type_names[std::type_index(typeid(int))] = "int";
    type_names[std::type_index(typeid(double))] = "double";
    type_names[std::type_index(typeid(A))] = "A";
    type_names[std::type_index(typeid(B))] = "B";
    type_names[std::type_index(typeid(C))] = "C";

    int i;
    double d;
    A a;

    auto x = 0.5min;
    std::chrono::duration<double, std::ratio<60>> y = 0.75min;
    std::cout << std::type_index(typeid(x)).name() << '\n';
    std::cout << std::type_index(typeid(y)).name() << '\n';

    // 注意我们正在存储指向类型 A 的指针
    std::unique_ptr<A> b(new B);
    std::unique_ptr<A> c(new C);

    auto &vb = (*b);
    auto &vc = (*c);
    std::cout << "i is " << type_names[std::type_index(typeid(i))] << '\n';
    std::cout << "d is " << type_names[std::type_index(typeid(d))] << '\n';
    std::cout << "a is " << type_names[std::type_index(typeid(a))] << '\n';
    std::cout << "b is " << type_names[std::type_index(typeid(vb))] << '\n';
    std::cout << "c is " << type_names[std::type_index(typeid(vc))] << '\n';
}

void test_variable() {
    using namespace std::chrono_literals;
    using namespace cmdr::opt::vars;
    //
}

int main() {
    std::cout << std::boolalpha;

    X x1{1};
    X x2{(int16_t) 2};
    X x3{(uint32_t) 3};
    X x4{true};
    X x5{"zed vvv"};
    X x6{std::string("zed vvv string")};

    std::cout // << std::boolalpha
            << x1 << x2 << x3 << x4 << x5 << std::endl
            << std::any_cast<std::string>(x6._arg) << std::endl;

    std::any a1{std::string("zed vvv string")};
    std::cout << "any: " << std::any_cast<std::string>(a1) << std::endl;

    test_streamer_any();
    test_streamable_any();
    test_types();
    test_variable();
}