//
//
//

#include "version.h"
#include <cmdr-cxx.hh>

void fatal_exit(const std::string &msg) {
    std::cerr << msg << '\n';
    exit(-1);
}

int main(int argc, char *argv[]) {
    try {
        using namespace cmdr::opt;

        auto cli = cmdr::cli("app2", xVERSION_STRING, "hedzr",
                             "Copyright © 2021 by hedzr, All Rights Reserved.",
                             "A demo app for cmdr-c11 library.",
                             "$ ~ --help");

        // cli.opt(opt_dummy{}());

        cli += sub_cmd{}("server", "s", "svr")
                       .description("server operations for listening");

        cli += opt{1}("count", "c")
                       .description("set counter value");

        cli += opt{""}("host", "H", "hostname", "server-name")
                       .description("hostname or ip address");

        cmdr::set("wudao.count", 1);
        cmdr::set("wudao.string", "str");
        cmdr::set("wudao.float", 3.14f);
        cmdr::set("wudao.double", 2.718);
        cmdr::set("wudao.array", std::vector{"a", "b", "c"});
        cmdr::set("wudao.bool", false);

        std::cout << cmdr::get<int>("wudao.count") << '\n';
        auto const &aa = cmdr::get<std::vector<char const *>>("wudao.array");
        std::cout << cmdr::string::join(aa, ", ", "[", "]") << '\n';
        cmdr::vars::variable &ab = cmdr::get_app().get("wudao.array");
        std::cout << ab << '\n';

        return cli.run(argc, argv);

    } catch (std::exception &e) {
        std::cerr << "Exception caught : " << e.what() << '\n';

    } catch (...) {
        cmdr::get_app().post_run(); // optional to post_run(), for the rare exception post processing if necessary
    }
}
