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

#if defined(_DEBUG)
    store.dump_full_keys(std::cout);
    store.dump_tree(std::cout);
#endif

    auto vv = store.get_raw("app.server.tls.handshake.max-idle-time");
    std::cout << "max-idle-time: " << vv << '\n';
    if (vv.as_string() != "45m")
        fatal_exit("  ^-- ERR: expect '45m'.");
}
