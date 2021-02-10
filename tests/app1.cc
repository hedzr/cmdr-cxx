//
//
//

#include "./addons/loaders/yaml_loader.hh"
#include "version.h"
#include <cmdr-cxx.hh>

#include <cmath>
#include <complex>
#include <fstream>

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>


void fatal_exit(const std::string &msg) {
    std::cerr << msg << '\n';
    exit(-1);
}

void add_sub1_menu(cmdr::app &cli, cmdr::opt::cmd &t1, const_chars title) {
    using namespace cmdr::opt;
    UNUSED(cli);

    t1 += sub_cmd{}(title)
                  .description((std::string(title) + " command").c_str());

    auto &ct1 = t1(title);
    {
        std::string prefix(title);
        std::string ps(prefix.substr(0, 1));
        ps += prefix.substr(prefix.length() - 1);
        prefix += "-";
        ct1 += opt{10}(prefix + "retry", ps + "r")
                       .description("set the retry times");
        ct1 += opt{9}(prefix + "retry1", ps + "r1")
                       .description("set the retry times");
        ct1 += opt{"saved-bash"}(prefix + "shell-name", ps + "sn")
                       .description("flag a");
        ct1 += opt{}(prefix + "arch", ps + "a")
                       .description("flag a");
        ct1 += opt{}(prefix + "bech", ps + "b")
                       .description("flag b");
        ct1 += opt{}(prefix + "coach", ps + "c")
                       .description("flag c");
        ct1 += opt{}(prefix + "dent", ps + "d")
                       .description("flag d");
        ct1 += opt{}(prefix + "etch", ps + "e")
                       .description("flag e");
    }
}

void add_test_menu(cmdr::app &cli) {
    using namespace cmdr::opt;

    cli += sub_cmd{}("test", "t", "test-command")
                   .description("main tests commands for testing")
                   .group("Test");
    {
        auto &t1 = *cli.last_added_command();
        t1 += sub_cmd{}("ueh", "ueh")
                      .description("causes a segfault for testing")
                      .group("Test")
                      .on_invoke([](cmd const, string_array const &) -> int {
                          int *foo = (int *) -1; // make a bad pointer
                          printf("%d\n", *foo);  // causes segfault
                          return 0;
                      });
    }


    cli += sub_cmd{}("main", "m", "main-command")
                   .description("main command for testing")
                   .group("Test");
    {
        auto &t1 = *cli.last_added_command();

        t1 += opt{10}("int", "i")
                      .description("set the int-value");
        t1 += opt{""}("string", "str")
                      .description("set the string-value");
        t1 += opt{}("b-arch", "a")
                      .description("flag a");
        t1 += opt{}("b-bech", "b")
                      .description("flag b");
        t1 += opt{}("b-coach", "c")
                      .description("flag c");
        t1 += opt{}("b-dent", "d")
                      .description("flag d");
        t1 += opt{std::vector{"a"}}("array", "e")
                      .description("flag array");

        using namespace std::complex_literals;

        t1 += opt{1i}("complex-1")
                      .description("complex-1 flag");
        t1 += opt{3. + 4i}("complex-2")
                      .description("complex-2 flag");

        add_sub1_menu(cli, t1, "sub1");
        add_sub1_menu(cli, t1, "sub2");
        add_sub1_menu(cli, t1, "sub3");
        add_sub1_menu(cli, t1, "sub4");

        auto &t2 = t1("sub2");
        add_sub1_menu(cli, t2, "mem-sub1");
        add_sub1_menu(cli, t2, "mem-sub2");
        add_sub1_menu(cli, t2, "mem-sub3");

        auto &t3 = t1("sub3");
        add_sub1_menu(cli, t3, "wow-bug1");
        add_sub1_menu(cli, t3, "wow-bug2");
        add_sub1_menu(cli, t3, "wow-bug3");

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

#if defined(_DEBUG)
        // std::cout << "test" << '\n';
        // std::any v1 = "lolo";
        // auto v1e = std::any_cast<char const *>(v1);
        auto &dv1 = t1.last_added_arg()->default_value();
        // std::cout << dv1.as_string() << ". type: " << dv1.type().name() << '\n';
#if defined(CAST_CONST_CHARS_AS_STD_STRING)
        auto dv2 = dv1->cast_as<std::string>();
#else
        auto dv2 = dv1->cast_as<char const *>();
#endif
        assert(std::string(dv2) == "localhost");
        // std::cout << "test: " << dv1 << v1e << '\n';
#endif

        t1 += opt{(int16_t) 4567}("port", "p")
                      .description("listening port number")
                      .group("TCP")
                      .placeholder("PORT")
                      .env_vars("PORT", "SERVER_PORT");

        t1 += sub_cmd{}("start", "s", "startup", "run")
                      .description("start the server as a daemon service, or run it at foreground")
                      // .group("")
                      .on_invoke([](cmdr::opt::cmd const &c, string_array const &remain_args) -> int {
                          UNUSED(c, remain_args);
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
    auto cli = cmdr::cli("app1", xVERSION_STRING, "hedzr",
                         "Copyright © 2021 by hedzr, All Rights Reserved.",
                         "A demo app for cmdr-c11 library.",
                         "$ ~ --help");

    try {
        using namespace cmdr::opt;

        // cli.opt(opt_dummy<support_types>{}());

        add_server_menu(cli);
        add_test_menu(cli);

#if defined(_DEBUG)
        auto &cc = cli("server");
        assert(cc.valid());
        assert(cc["count"].valid());
        assert(cc["host"].valid());
        assert(cc("status").valid());
        assert(cc("start").valid());
        assert(cc("run", true).valid());
#endif

#if CMDR_TEST_ON_COMMAND_NOT_HOOKED
        cli.set_global_on_command_not_hooked([](cmdr::opt::cmd const &, string_array const &) {
            cmdr::get_store().dump_full_keys(std::cout);
            cmdr::get_store().dump_tree(std::cout);
            return 0;
        });
#endif

#if 1
        {
            using namespace cmdr::addons::loaders;
            cli.set_global_on_loading_externals(yaml_loader{}());
        }
#endif

#if 1
        cli += sub_cmd{}("dup", "dup")
                       .description("dup command/flag for testing")
                       .group("Test");
        {
            auto &t1 = *cli.last_added_command();

            t1 += opt{10}("int", "i")
                          .description("set the int-value");
            t1 += opt{10}("int", "i")
                          .description("set the int-value");
            t1 += opt{""}("string", "str")
                          .description("set the string-value");
        }
#endif

    } catch (std::exception &e) {
        std::cerr << "Exception caught for testing (NOT BUG) : " << e.what() << '\n';
        CMDR_DUMP_STACK_TRACE(e);
    }

    cmdr::debug::UnhandledExceptionHookInstaller _ueh{};
    return cli.run(argc, argv);
}
