//
// Created by Hedzr Yeh on 2021/1/21.
//

#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <new>
#include <string>
#include <vector>

// #include "cmdr11/cmdr_defs.hh"
#include "cmdr11/cmdr_var_t.hh"


void fatal_exit(const std::string &msg) {
    std::cerr << msg << '\n';
    exit(-1);
}

void test_store_1() {
    using namespace std::chrono_literals;

    std::cout << "\nstore testing... \n\n";

    cmdr::vars::store store;

    store.set_raw("app.server.tls.enabled", true);
    store.set_raw("app.server.tls.ca.cert", "ca.cer");
    store.set_raw("app.server.tls.server.cert", "server.cer");
    store.set_raw("app.server.tls.server.key", "server.key");
    store.set_raw("app.server.tls.client-auth", true);
    store.set_raw("app.server.tls.handshake.timeout", 10s);
    store.set_raw("app.server.tls.handshake.max-idle-time", 45min);
    store.set_raw("app.server.tls.domains", std::vector{"example.com", "example.org"});
    store.set_raw("app.server.tls.fixed-list", std::array{"item1", "item2"});

    store.set_raw("app.server.test.test-1-time", 0.75min);
    store.set_raw("app.server.test.test-2-time", 501ns);
    store.set_raw("app.server.test.test-3-time", 730us);
    store.set_raw("app.server.test.test-4-time", 233ms);
    store.set_raw("app.server.test.test-5-time", 7s);
    store.set_raw("app.server.test.test-6-time", 7.2s);
    store.set_raw("app.server.test.test-7-time", 1024h);

    store.set_raw("app.server.ii-1", 123u);
    store.set_raw("app.server.ii-2", 7.0);
    store.set_raw("app.server.ii-3", 3.14159f);
    store.set_raw("app.server.ii-4", 2.71828);
    store.set_raw("app.server.ii-5", "a string");
    store.set_raw("app.server.vv-1", std::vector{"a string", "b", "c"});

#if defined(_DEBUG)
    store.dump_full_keys(std::cout);
    store.dump_tree(std::cout);
#endif

    auto vv = store.get_raw("app.server.tls.handshake.max-idle-time");
    std::cout << "max-idle-time: " << vv << '\n';
    if (vv.as_string() != "45m")
        fatal_exit("  ^-- ERR: expect '45m'.");
}
