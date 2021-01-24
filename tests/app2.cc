//
//
//

#include "version.h"
#include <cmdr-cxx.hh>

void fatal_exit(const std::string &msg) {
    std::cerr << msg << '\n';
    exit(-1);
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

#if defined(_DEBUG)
        // auto &store = cli.store();
        auto &store = cmdr::get_store();

        using namespace std::chrono_literals;

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

        auto vv = store.get("app.server.tls.handshake.max-idle-time");
        std::cout << "max-idle-time: " << vv << '\n';
        if (vv.as_string() != "45m")
            fatal_exit("  ^-- ERR: expect '45m'.");
#endif

        cmd &cc = cli("server");
        assert(cc["count"].valid());
        assert(cc["host"].valid());
        assert(cc("status").valid());
        assert(cc("start").valid());
        assert(cc("run", true).valid());

        using namespace cmdr::terminal::colors;
        // auto &c = colorize::instance();
        auto c = colorize::create();
        std::cout << c.fg(c.Purple3).bg(c.Default).underline().bold().s("some text") << '\n';
        std::cout << c.dim().s("dim text") << '\n';
        std::cout << c.Purple3 << " << want 56\n";
        std::cout << c.MediumTurquoise << " << want 80\n";
        std::cout << c.Grey93 << " << want 255\n";

        using cmdr::terminal::colors::colorize;
        std::cout << colorize::style::underline << colorize::fg::red << "Hello, Colorful World!" << std::endl;
        std::cout << colorize::reset::all << "Here I'm!" << std::endl;
        std::cout << "END.\n\n";

        std::cout << "How many colors: " << cmdr::terminal::terminfo::support_colors() << '\n';
        std::cout << "Terminal: " << cmdr::terminal::terminfo::term() << '\n';
        return cli.run(argc, argv);

    } catch (std::exception &e) {
        std::cerr << "Exception caught : " << e.what() << std::endl;

    } catch (...) {
        cmdr::get_app().post_run(); // optional to post_run(), for the rare exception post processing if necessary
    }
}
