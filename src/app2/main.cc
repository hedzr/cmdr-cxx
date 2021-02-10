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

        cli += opt{""}("host", "h", "hostname", "server-name")
                       .description("hostname or ip address");

        return cli.run(argc, argv);

    } catch (std::exception &e) {
        std::cerr << "Exception caught : " << e.what() << '\n';

    } catch (...) {
        cmdr::get_app().post_run(); // optional to post_run(), for the rare exception post processing if necessary
    }
}
