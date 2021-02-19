//
// Created by Hedzr Yeh on 2021/1/20.
//
#include "visit_any.hh"
#include <cmdr-cxx.hh>

#include "second_src.hh"


void second_test() {
    std::cout << "second\n";
}


void fatal_exit(const std::string &msg) {
    std::cerr << msg << '\n';
    exit(-1);
}

void add_sub1_menu(cmdr::app &cli, cmdr::opt::cmd &t1, const_chars title) {
    using namespace cmdr::opt;
    UNUSED(cli, t1);

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
        auto &t1 = *(cli.last_added_command());
        t1 += sub_cmd{}("ueh", "ueh", "c++=excetion")
                      .description("causes a runtime-error and hook it via std c++11 way")
                      .group("Test")
                      .on_invoke([](cmd const, string_array const &) -> int {
        //
        // no effects because we try and catch all c++ exceptions in app::run()
        //
#if __clang__ || __GNUC__
                          cmdr::debug::UnhandledExceptionHookInstaller _ueh{};
#endif
                          throw std::runtime_error("hello, this is a runtime_error");
                          return 0;
                      });
        t1 += sub_cmd{}("ssi", "ssi", "access-ileggal-address")
                      .description("causes a segfault and hook it via linux signal()")
                      .group("Test")
                      .on_invoke([](cmd const, string_array const &) -> int {
#if __clang__ || __GNUC__
                          // cmdr::debug::UnhandledExceptionHookInstaller _ueh{};
                          cmdr::debug::SigSegVInstaller _ssi{};
#endif
                          int *foo = (int *) -1; // make a bad pointer
                          printf("%d\n", *foo);  // causes segfault
                          return 0;
                      });
        t1 += sub_cmd{}("dbz", "dbz", "divide-by-zero")
                      .description("causes a segfault and hook it via linux signal()")
                      .group("Test")
                      .on_invoke([](cmd const, string_array const &) -> int {
#if __clang__ || __GNUC__
                          // cmdr::debug::UnhandledExceptionHookInstaller _ueh{};
                          cmdr::debug::SignalInstaller<SIGFPE> _si{};
#endif
                          int foo = 0;
                          printf("%d\n", 8989 / foo); // causes segfpe
                          return 0;
                      });
    }
}

void add_main_menu(cmdr::app &cli) {
    using namespace cmdr::opt;

    cli += sub_cmd{}("main", "m", "main-command")
                   .description("main command for testing")
                   .group("Test");
    {
        auto &t1 = *cli.last_added_command();

        auto ox1 = opt{79129L};
        ox1("long", "b")
                .description("flag b")
                .required();
        // std::cout << "1. " << *ox1.underlying().default_value().get() << '\n';
        t1 += ox1;
        // // std::cout << "2. " << *ox1.underlying().default_value().get() << '\n';
        // // std::cout << "3. " << *t1["long"].default_value().get() << '\n';
        // std::cout << "4. CLI/STORE: " << cmdr::get_for_cli<long>("main.long") << '\n';

        t1 += opt{10}("int", "i")
                      .description("set the int-value");
        t1 += opt{""}("string", "str")
                      .description("set the string-value");
        t1 += opt{3.13f}("float", "f")
                      .description("flag float");
        t1 += opt{3.12}("double", "d")
                      .description("flag double");

        t1 += opt{std::vector{3, 9, 17}}("int-array", "ia")
                      .description("flag int array");
        t1 += opt{98LL}("long-long", "ll")
                      .description("flag long-long");
        t1 += opt{std::vector{"a", "Z"}}("string-array", "sa")
                      .description("flag string array");

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
        // std::cout << dv1.as_string() << ". type: " << dv1.type().name() << '\n';
        // std::cout << "test" << '\n';
        // std::any v1 = "lolo";
        // auto v1e = std::any_cast<char const *>(v1);
        auto &dv1 = t1.last_added_arg()->default_value();
#if defined(CMDR_CAST_CONST_CHARS_AS_STD_STRING)
        auto dv2 = dv1->cast_as<std::string>();
#else
        auto dv2 = dv1->cast_as<char const *>();
#endif
        CMDR_ASSERT(std::string(dv2) == "localhost");
        // std::cout << "test: " << dv1 << v1e << '\n';
#endif

        t1 += opt{(int16_t) 4567}("port", "p", "pt", "prt")
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
