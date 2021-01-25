//
//
//

#include "version.h"
#include <cmdr-cxx.hh>

void fatal_exit(const std::string &msg) {
    std::cerr << msg << '\n';
    exit(-1);
}

void add_test_menu(cmdr::opt::app &cli) {
    using namespace cmdr::opt;

    cli += subcmd{}
                   .titles("main", "m", "main-command")
                   .description("main command for testing")
                   .group("Test")
                   .opt(opt_dummy{}())
                   .opt(opt_dummy{}());
    {
        auto &t1 = *cli.last_added_command();

        t1 += opt{10}
                      .titles("retry", "r")
                      .description("set the retry times");
        t1 += opt{9}
                .titles("retry1", "r1")
                .description("set the retry times");
        t1 += opt<bool>{}
                .titles("arch", "a")
                .description("flag a");
        t1 += opt<bool>{}
                .titles("bech", "b")
                .description("flag b");
        t1 += opt<bool>{}
                .titles("coach", "c")
                .description("flag c");
        t1 += opt<bool>{}
                .titles("dent", "d")
                .description("flag d");
        t1 += opt<bool>{}
                .titles("etch", "e")
                .description("flag e");

        t1 += subcmd{}
                .titles("sub1", "s1", "svr1")
                .description("sub1 command")
                ;
        t1 += subcmd{}
                .titles("sub2", "s2", "svr2")
                .description("sub2 command")
                ;

        auto ct1 = t1("sub1");
        {
            ct1 += opt{10}
                          .titles("retry", "r")
                          .description("set the retry times");
            ct1 += opt{9}
                          .titles("retry1", "r1")
                          .description("set the retry times");
            ct1 += opt<bool>{}
                          .titles("arch", "a")
                          .description("flag a");
            ct1 += opt<bool>{}
                          .titles("bech", "b")
                          .description("flag b");
            ct1 += opt<bool>{}
                          .titles("coach", "c")
                          .description("flag c");
            ct1 += opt<bool>{}
                          .titles("dent", "d")
                          .description("flag d");
            ct1 += opt<bool>{}
                          .titles("etch", "e")
                          .description("flag e");
        }
        auto ct2 = t1("sub2");
        {
            ct2 += opt{-1}
                    .titles("retry", "r")
                    .description("set the retry times");
            ct2 += opt{9}
                    .titles("retry1", "r1")
                    .description("set the retry times");
            ct2 += opt<bool>{}
                    .titles("arch", "a")
                    .description("flag a");
            ct2 += opt<bool>{}
                    .titles("bech", "b")
                    .description("flag b");
            ct2 += opt<bool>{}
                    .titles("coach", "c")
                    .description("flag c");
            ct2 += opt<bool>{}
                    .titles("dent", "d")
                    .description("flag d");
            ct2 += opt<bool>{}
                    .titles("etch", "e")
                    .description("flag e");
        }
    }
}

void add_server_menu(cmdr::opt::app &cli) {
    using namespace cmdr::opt;

    cli += subcmd{}
                   .titles("server", "s", "svr")
                   .description("server operations for listening")
                   .group("TCP/UDP/Unix")
                   .opt(opt_dummy{}())
                   .opt(opt_dummy{}());
    {
        auto &t1 = *cli.last_added_command();

        t1 += opt{(int16_t)(8)}
                      .titles("retry", "r")
                      .description("set the retry times")
                // .default_value(cmdr::support_types((int16_t)(7)))
                ;

        t1 += opt{(uint64_t) 2}
                      .titles("count", "c")
                      .description("set counter value");

        t1 += opt{"localhost"}
                      .titles("host", "H", "hostname", "server-name")
                      .description("hostname or ip address")
                      .group("TCP")
                      .placeholder("HOST[:IP]");

        t1 += opt{(int16_t) 4567}
                      .titles("port", "p")
                      .description("listening port number")
                      .group("TCP")
                      .placeholder("PORT")
                      .env_vars("PORT", "SERVER_PORT");

        t1 += subcmd{}
                      .titles("start", "s", "startup", "run")
                      .description("start the server as a daemon service, or run it at foreground")
                      // .group("")
                      .opt(opt_dummy{}())
                      .opt(opt_dummy{}())
                      .on_invoke([](cmd const &c, string_array const &remain_args) -> int {
                          unused(c);
                          unused(remain_args);
                          std::cout << c.title() << " invoked.\n";
                          return 0;
                      });

        t1 += subcmd{}
                      .titles("stop", "t", "shutdown")
                      .description("stop the daemon service, or stop the server");

        t1 += subcmd{}
                      .titles("pause", "p")
                      .description("pause the daemon service");

        t1 += subcmd{}
                      .titles("resume", "re")
                      .description("resume the paused daemon service");
        t1 += subcmd{}
                      .titles("reload", "r")
                      .description("reload the daemon service");
        t1 += subcmd{}
                      .titles("hot-reload", "hr")
                      .description("hot-reload the daemon service without stopping the process");
        t1 += subcmd{}
                      .titles("status", "st", "info", "details")
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

        cli.opt(opt_dummy{}());

        // add_global_options(cli);
        // add_generator_menu(cli);
        add_server_menu(cli);
        add_test_menu(cli);

        cmd &cc = cli("server");
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
