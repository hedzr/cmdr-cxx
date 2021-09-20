//
// Created by Hedzr Yeh on 2021/1/17.
//

#include <any>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <new>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>


#include "cmdr11/cmdr_defs.hh"
#include "cmdr11/cmdr_utils.hh"
#include "cmdr11/cmdr_var_t.hh"

#include "cmdr11/cmdr_x_test.hh"

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
        std::cout << ".1." << a0 << '\n';
    }

    template<typename A,
             std::enable_if_t<!std::is_same<std::decay_t<A>, std::any>::value &&
                                      !std::is_same<std::decay_t<A>, X>::value,
                              int> = 0>
    explicit X(A &&v)
        : _arg(std::forward<A>(v)) {
        std::cout << ".2." << v << '\n';
    }

    friend std::ostream &operator<<(std::ostream &os, X const &a) {
        (void) a; // std::cout << a._arg;
        return os;
    }

    std::any _arg;
};

void test_streamer_any() {
#if OBSELETED
    using cmdr::util::streamer_any;
    using cmdr::vars::streamable_any;
    using namespace std::chrono_literals;

    std::cout << "test_streamable_any manually:" << '\n';

    streamable_any vo1{1};
    streamable_any vo2{true};
    streamable_any vo3{1.3};
    streamable_any vo4{(int32_t) -7};
    streamable_any vo5{"a string"};

    std::cout << vo1 << '\n';
    std::cout << vo2 << '\n';
    std::cout << vo3 << '\n';
    std::cout << vo4 << '\n';
    std::cout << vo5 << '\n';

    std::cout << '\n';
#endif
}

void test_streamable_any() {
#if OBSELETED
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
        // std::cout << v << '\n';
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
#endif
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
    // using namespace std::chrono_literals;
    // using namespace cmdr::vars;
    //
    // variable v;
    // std::stringstream ss("false");
    // ss >> v;
    //
    // std::cout << v << '\n';
}

#include <climits>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>


namespace fold {
    template<typename... Args>
    bool all(Args... args) { return (... && args); }

    template<typename... Args>
    int sum(Args &&...args) {
        //    return (args + ... + 1 * 2); // 错误：优先级低于转型的运算符
        return (args + ... + (1 * 2)); // OK
    }

    template<class... Args>
    void printer(Args &&...args) {
        (std::cout << ... << args) << '\n';
    }

    template<class T, class... Args>
    void push_back_vec(std::vector<T> &v, Args &&...args) {
        static_assert((std::is_constructible_v<T, Args &> && ...));
        (v.push_back(args), ...);
    }

    // 基于 http://stackoverflow.com/a/36937049 的编译时端序交换
    template<class T, std::size_t... N>
    constexpr T bswap_impl(T i, std::index_sequence<N...>) {
        return (((i >> N * CHAR_BIT & std::uint8_t(-1)) << (sizeof(T) - 1 - N) * CHAR_BIT) | ...);
    }

    template<class T, class U = std::make_unsigned_t<T>>
    constexpr U bswap(T i) {
        return bswap_impl<U>(i, std::make_index_sequence<sizeof(T)>{});
    }
} // namespace fold

void test_fold() {
#if defined(_DEBUG)
    {
        bool b = fold::all(true, true, true, false);
        CMDR_ASSERT(!b);
    }
#endif

    fold::printer(1, 2, 3, "abc");

    std::vector<int> v;
    fold::push_back_vec(v, 6, 2, 45, 12);
    fold::push_back_vec(v, 1, 2, 9);
    for (int i : v) std::cout << i << ' ';

    static_assert(fold::bswap<std::uint16_t>(0x1234u) == 0x3412u);
    static_assert(fold::bswap<std::uint64_t>(0x0123456789abcdefULL) == 0xefcdab8967452301ULL);
}

// https://stackoverflow.com/a/7943736/6375060
void main_lambda_compare() {
    auto l = [](int i) { return long(i); };
    typedef cmdr::debug::lambda_func_type<decltype(l)>::type T;
    static_assert(std::is_same<T, long(int) const>::value, "ok");
}

void test_variant() {
    // cmdr::support_types v1(std::vector{"example", "no body"});
    // (void) v1;

    using namespace std::chrono_literals;
    using namespace cmdr::vars;

    std::cout << '\n'
              << "variable testing...." << '\n';

    variable v(true);
    std::stringstream ss("false");
    ss >> v;
    std::cout << v << '\n';

    v = "";
    ss << "string is ok";
    ss >> v;
    std::cout << v << '\n';

    auto g = [](std::istream &, std::any &) {};
    std::cout << typeid(g).name() << '\n';
    std::cout << cmdr::debug::type_name<decltype(g)>() << '\n';
    if constexpr (std::is_same_v<cmdr::debug::lambda_func_type<decltype(g)>::type, void(std::istream &, std::any &) const>) {
        std::cout << "matched!\n";
    }
}

void test_tuple_to_string() {
    auto tup = std::make_tuple(1, "hello", 4.5);
    std::cout << tup << '\n';
}

void test_vector_to_string() {
    auto vec = {"a", "b", "v"};
    std::cout << vec << '\n';
}

int main() {
    std::cout << "bool test: " << std::boolalpha << true << '\n';

    CMDR_TEST_FOR(test_tuple_to_string);
    CMDR_TEST_FOR(test_vector_to_string);
    
    X x1{1};
    X x2{(int16_t) 2};
    X x3{(uint32_t) 3};
    X x4{true};
    X x5{"zed vvv"};
    X x6{std::string("zed vvv string")};

    std::cout // << std::boolalpha
            << x1 << x2 << x3 << x4 << x5 << '\n'
            << std::any_cast<std::string>(x6._arg) << '\n';

    std::any a1{std::string("zed vvv string")};
    std::cout << "any: " << std::any_cast<std::string>(a1) << '\n';

    test_streamer_any();
    test_streamable_any();
    test_types();
    test_variable();
    test_fold();
    test_variant();
}