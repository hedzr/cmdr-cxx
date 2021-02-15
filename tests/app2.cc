//
//
//

// #include "./addons/loaders/yaml_loader.hh"
#include <cmdr-cxx.hh>

#include <cmath>
#include <complex>
#include <fstream>

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "second_src.hh"

int main(int argc, char *argv[]) {
    auto &cli = cmdr::cli("app2", CMDR_VERSION_STRING, "hedzr",
                         "Copyright © 2021 by hedzr, All Rights Reserved.",
                         "A demo app for cmdr-c11 library.",
                         "$ ~ --help")
                       // remove "Powered by cmdr-cxx" line
                       .set_no_cmdr_endings()
                       // customize the last line except cmdr endings
                       .set_tail_line("")
                       .set_no_tail_line(false);

    try {
        using namespace cmdr::opt;

        // cli.opt(opt_dummy<support_types>{}());

        add_server_menu(cli);
        add_test_menu(cli);
        add_main_menu(cli);

        // cli.set_minimal_tab_stop(55);

#if defined(_DEBUG)
        auto &cc = cli("server");
        CMDR_ASSERT(cc.valid());
        CMDR_ASSERT(cc["count"].valid());
        CMDR_ASSERT(cc["host"].valid());
        CMDR_ASSERT(cc("status").valid());
        CMDR_ASSERT(cc("start").valid());
        CMDR_ASSERT(cc("run", true).valid());
#endif

#if CMDR_TEST_ON_COMMAND_NOT_HOOKED
        cli.set_global_on_command_not_hooked([](cmdr::opt::cmd const &, string_array const &) {
            cmdr::get_store().dump_full_keys(std::cout);
            cmdr::get_store().dump_tree(std::cout);
            return 0;
        });
#endif

#if 0
        {
            using namespace cmdr::addons::loaders;
            cli.set_global_on_loading_externals(yaml_loader{}());
        }
#endif

#if 0
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

    // cmdr::debug::UnhandledExceptionHookInstaller _ueh{};
    // cmdr::debug::SigSegVInstaller _ssi{};
    return cli.run(argc, argv);
}
