//
//
//

#include <cmdr-cxx.hh>


int main(int argc, char *argv[]) {
#if defined(VERBOSE_DEBUG) || 1
#if defined(_DEBUG)
    // extern void small_cases();
    // small_cases();
#endif
#endif

    try {
        using namespace cmdr::opt;
        return cmdr::cli("app1", CMDR_VERSION_STRING, "hedzr",
                         "Copyright © 2021 by hedzr, All Rights Reserved.",
                         "A demo app for cmdr-c11 library.",
                         "$ ~ --help")
                .opt(sub_cmd{}("server", "s", "svr")
                             .description("server operations for listening")
                             .get())
                .opt(opt{(int16_t)(3)}("count", "c")
                             .description("set counter value")
                             .get())
                .opt(sub_cmd{}("host", "h", "hostname", "server-name")
                             .description("hostname or ip address")
                             .get())
                .run(argc, argv);
    } catch (std::exception &e) {
        std::cerr << "Exception caught : " << e.what() << '\n';
    }
}
