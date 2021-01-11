//
// Created by Hedzr Yeh on 2021/1/17.
//

#include <cstdio>
#include <cstdlib>
#include <new>
#include <string>

#include "cmdr11/cmdr_var_t.hh"


std::size_t allocated = 0;

void *operator new(size_t sz) {
    void *p = std::malloc(sz);
    allocated += sz;
    return p;
}

void operator delete(void *p) noexcept {
    return std::free(p);
}

int main() {
    allocated = 0;
    std::string s("hi");
    std::printf("stack space = %zu, heap space = %zu, capacity = %zu\n",
                sizeof(s), allocated, s.capacity());

    cmdr::opt::vars::var_t<std::string> vs;
    std::cout << "var_t: " << vs << std::endl;


    using namespace std::chrono_literals;

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

    std::cout << "store: " << store << std::endl;

#if defined(_DEBUG)
    store.root().dump_full_keys(std::cout);
    store.root().dump_tree(std::cout);
#endif

    std::cout << std::boolalpha;
    std::cout << cmdr::is_vector_v<std::vector<int>> << std::endl;
    std::cout << cmdr::is_vector_v<int> << std::endl;
    std::cout << cmdr::is_container<std::string>::value << std::endl;
    std::cout << cmdr::is_container<std::list<std::string>>::value << std::endl;

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