//
//
//

#include "version.h"
#include <cmdr-cxx.hh>


int main(int argc, char *argv[]) {
#if defined(VERBOSE_DEBUG) || 1
#if defined(_DEBUG)
    extern void small_cases();
    small_cases();
#endif
#endif

    try {
        return cmdr::create_app("app1", xVERSION_STRING, "hedzr",
                                "Copyright © 2021 by hedzr, All Rights Reserved.",
                                "A demo app for cmdr-c11 library.",
                                "$ ~ --help")
                .option(cmdr::opt::opt_dummy{}())
                .option(cmdr::opt::opt_subcmd{}
                                .titles("server", "s", "svr")
                                .description("server operations for listening")
                                .option(cmdr::opt::opt_dummy{}())
                                .option(cmdr::opt::opt_dummy{}())
                                .get())
                .option(cmdr::opt::opt_int{}
                                .titles("count", "c")
                                .description("set counter value")
                                .default_value(cmdr::support_types((int16_t)(3)))())
                .option(cmdr::opt::opt_string{}
                                .titles("host", "h", "hostname", "server-name")
                                .description("hostname or ip address")())
                .run(argc, argv);
    } catch (std::exception &e) {
        std::cerr << "Exception caught : " << e.what() << std::endl;
    } catch (...) {
        cmdr::get_app().post_run(); // optional to post_run(), for the rare exception post processing if necessary

        // if (std::current_exception() != nullptr) {
        //     std::cerr << "Exception caught : " << e.what() << std::endl;
        // } else {
        //     if (cmdr::get_app().help_hit()) {
        //         //
        //     } else {
        //         //
        //     }
        // }
    }
}
