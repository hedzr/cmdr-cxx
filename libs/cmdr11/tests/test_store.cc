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
