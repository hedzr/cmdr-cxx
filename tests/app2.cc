//
//
//

#include "version.h"
#include <cmdr-cxx.hh>

void fatal_exit(const std::string &msg) {
    std::cerr << msg << '\n';
    exit(-1);
}

void add_sub1_menu(cmdr::app &cli, cmdr::opt::cmd &t1, const_chars title) {
    using namespace cmdr::opt;
    unused(cli);

    t1 += sub_cmd{}(title)
                  .description((std::string(title) + " command").c_str());

    auto &ct1 = t1(title);
    {
        ct1 += opt{10}("retry", "r")
                       .description("set the retry times");
        ct1 += opt{9}("retry1", "r1")
                       .description("set the retry times");
        ct1 += opt{"saved-bash"}("shell-name", "sn")
                       .description("flag a");
        ct1 += opt{}("arch", "a")
                       .description("flag a");
        ct1 += opt{}("bech", "b")
                       .description("flag b");
        ct1 += opt{}("coach", "c")
                       .description("flag c");
        ct1 += opt{}("dent", "d")
                       .description("flag d");
        ct1 += opt{}("etch", "e")
                       .description("flag e");
    }
}

void add_test_menu(cmdr::app &cli) {
    using namespace cmdr::opt;

    cli += sub_cmd{}("main", "m", "main-command")
                   .description("main command for testing")
                   .group("Test");
    {
        auto &t1 = *cli.last_added_command();

        t1 += opt{10}("retry", "r")
                      .description("set the retry times");
        t1 += opt{9}("retry1", "r1")
                      .description("set the retry times");
        t1 += opt{}("arch", "a")
                      .description("flag a");
        t1 += opt{}("bech", "b")
                      .description("flag b");
        t1 += opt{}("coach", "c")
                      .description("flag c");
        t1 += opt{}("dent", "d")
                      .description("flag d");
        t1 += opt{}("etch", "e")
                      .description("flag e");

        add_sub1_menu(cli, t1, "sub1");
        add_sub1_menu(cli, t1, "sub2");
        add_sub1_menu(cli, t1, "sub3");
        add_sub1_menu(cli, t1, "sub4");

        auto &t2 = t1("sub2");
        add_sub1_menu(cli, t2, "sub-sub1");
        add_sub1_menu(cli, t2, "sub-sub2");
        add_sub1_menu(cli, t2, "sub-sub3");

        auto &t3 = t1("sub3");
        add_sub1_menu(cli, t3, "sub-bug1");
        add_sub1_menu(cli, t3, "sub-bug2");
        add_sub1_menu(cli, t3, "sub-bug3");

        auto &t4 = t1("sub4");
        add_sub1_menu(cli, t4, "bug-bug1");
        add_sub1_menu(cli, t4, "bug-bug2");

        auto &t42 = t4("bug-bug2");
        add_sub1_menu(cli, t42, "zero-sub1");
        add_sub1_menu(cli, t42, "zero-sub2");
        add_sub1_menu(cli, t42, "zero-sub3");
        add_sub1_menu(cli, t42, "zero-sub4");
        add_sub1_menu(cli, t42, "zero-sub5");
    }
}

void add_server_menu(cmdr::app &cli) {
    using namespace cmdr::opt;

    cli += sub_cmd{}("server", "s", "svr")
                   .description("server operations for listening")
                   .group("TCP/UDP/Unix");
    {
        auto &t1 = *cli.last_added_command();

        t1 += opt{(int16_t)(8)}("retry", "r")
                      .description("set the retry times")
                // .default_value(cmdr::support_types((int16_t)(7)))
                ;

        t1 += opt{(uint64_t) 2}("count", "c")
                      .description("set counter value");

        t1 += opt{"localhost"}("host", "H", "hostname", "server-name")
                      .description("hostname or ip address")
                      .group("TCP")
                      .placeholder("HOST[:IP]")
                      .env_vars("HOST");

        std::cout << "test" << '\n';
        std::any v1 = "lolo";
        auto v1e = std::any_cast<char const *>(v1);
        auto &dv1 = t1.last_added_arg()->default_value();
        std::cout << dv1.as_string() << ". type: " << dv1.type().name() << '\n';
#if defined(CAST_CONST_CHARS_AS_STD_STRING)
        auto dv2 = dv1.cast_as<std::string>();
#else
        auto dv2 = dv1.cast_as<char const *>();
#endif
        assert(std::string(dv2) == "localhost");
        std::cout << "test: " << dv1 << v1e << '\n';

        t1 += opt{(int16_t) 4567}("port", "p")
                      .description("listening port number")
                      .group("TCP")
                      .placeholder("PORT")
                      .env_vars("PORT", "SERVER_PORT");

        t1 += sub_cmd{}("start", "s", "startup", "run")
                      .description("start the server as a daemon service, or run it at foreground")
                      // .group("")
                      .on_invoke([](cmdr::opt::cmd const &c, string_array const &remain_args) -> int {
                          unused(c);
                          unused(remain_args);
                          std::cout << c.title() << " invoked.\n";
                          return 0;
                      });

        t1 += sub_cmd{}("stop", "t", "shutdown")
                      .description("stop the daemon service, or stop the server");

        t1 += sub_cmd{}("pause", "p")
                      .description("pause the daemon service");

        t1 += sub_cmd{}("resume", "re")
                      .description("resume the paused daemon service");
        t1 += sub_cmd{}("reload", "r")
                      .description("reload the daemon service");
        t1 += sub_cmd{}("hot-reload", "hr")
                      .description("hot-reload the daemon service without stopping the process");
        t1 += sub_cmd{}("status", "st", "info", "details")
                      .description("display the running status of the daemon service");
    }
}

int main(int argc, char *argv[]) {
    try {
        using namespace cmdr::opt;

        auto cli = cmdr::cli("app2", xVERSION_STRING, "hedzr",
                             "Copyright © 2021 by hedzr, All Rights Reserved.",
                             "A demo app for cmdr-c11 library.",
                             "$ ~ --help");

        // cli.opt(opt_dummy<support_types>{}());

        // add_global_options(cli);
        // add_generator_menu(cli);
        add_server_menu(cli);
        add_test_menu(cli);

        auto &cc = cli("server");
        assert(cc["count"].valid());
        assert(cc["host"].valid());
        assert(cc("status").valid());
        assert(cc("start").valid());
        assert(cc("run", true).valid());

        return cli.run(argc, argv);

    } catch (std::exception &e) {
        std::cerr << "Exception caught : " << e.what() << std::endl;

    } catch (...) {
        cmdr::get_app().post_run(); // optional to post_run(), for the rare exception post processing if necessary
    }
}
