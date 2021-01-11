//
// Created by Hedzr Yeh on 2020/11/19.
//

#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <new>
#include <string>
#include <vector>

#include "cmdr11/cmdr_var_t.hh"

#if JUST_FOR_REFERRING
int main(int, char **) {
    std::cout << "Running tests...\n";

    std::string input = "tests and strings";
    std::string cstr = compress_string(input);
    if (cstr != "bad one") {
        return 1;
    }

    std::string dstr = decompress_string(cstr);

    return dstr == cstr ? 0 : 1;
}
#endif

void test_types_check_1() {
    std::cout << std::boolalpha;

    std::cout << "is_vector_t: \n";
    std::cout << cmdr::is_vector_v<std::vector<int>> << std::endl;
    std::cout << cmdr::is_vector_v<int> << std::endl;

    std::cout << "is_container: \n";
    std::cout << cmdr::is_container<std::string>::value << std::endl;
    std::cout << cmdr::is_container<std::list<std::string>>::value << std::endl;

    using namespace std::chrono_literals;

    auto v1 = std::vector{"example.com", "example.org"};
    std::cout << "cmdr::is_stl_container : " << cmdr::is_stl_container<decltype(v1)>::value << std::endl;
    auto v2 = 10s;
    std::cout << "type " << typeid(v2).name() << ". check: " << cmdr::is_duration<decltype(v2)>::value << std::endl;

    std::cout << "cmdr::is_duration<std::chrono::seconds> : " << cmdr::is_duration<std::chrono::seconds>::value << std::endl;
    std::cout << "cmdr::is_duration<std::chrono::hours> : " << cmdr::is_duration<std::chrono::hours>::value << std::endl;
    std::cout << "cmdr::is_duration<std::chrono::microseconds> : " << cmdr::is_duration<std::chrono::microseconds>::value << std::endl;
    std::cout << "cmdr::is_duration<std::chrono::nanoseconds> : " << cmdr::is_duration<std::chrono::nanoseconds>::value << std::endl;
    std::cout << "cmdr::is_duration<long long> : " << cmdr::is_duration<decltype(0xfull)>::value << std::endl;
    std::cout << "cmdr::is_duration<bool> : " << cmdr::is_duration<bool>::value << std::endl;
}

void test_var_t_1() {
    cmdr::opt::vars::var_t<std::string> vs;
    std::cout << "var_t: " << vs << std::endl;
}

void test_store_1() {
    using namespace std::chrono_literals;

    std::cout << "\nstore testing... \n\n";

    cmdr::opt::vars::store<> store;
    store.set("app.server.tls.enabled", true);
    store.set("app.server.tls.ca.cert", "ca.cer");
    store.set("app.server.tls.server.cert", "server.cer");
    store.set("app.server.tls.server.key", "server.key");
    store.set("app.server.tls.client-auth", true);
    store.set("app.server.tls.handshake.timeout", 10s);
    store.set("app.server.tls.handshake.max-idle-time", 45min);
    store.set("app.server.tls.domains", std::vector{"example.com", "example.org"});
    store.set("app.server.tls.fixed-list", std::array{"item1", "item2"});

#if defined(_DEBUG)
    store.root().dump_full_keys(std::cout);
    store.root().dump_tree(std::cout);
#endif
}

int main(int, char **) {
    std::cout << "Running tests..." << std::endl;

    //    std::string input = "tests and strings";
    //    std::string cstr = compress_string(input);
    //
    //    //    if (cstr != "bad one") {
    //    //        std::cerr << "PRB\n";
    //    //        return 1;
    //    //    }
    //
    //    std::string dstr = decompress_string(cstr);

    auto *v = new std::vector<int>();
    v->push_back(1);
    v->push_back(2);
    v->push_back(3);
    v->push_back(4);

    //    int ret = dstr == input ? 0 : 1;
    int ret = v->size() == 4 ? 0 : 1;
    std::cout << "return " << ret << ", " << v->size() << std::endl;


    test_types_check_1();

    test_var_t_1();

    test_store_1();

    return ret;
}