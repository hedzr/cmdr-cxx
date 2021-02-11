//
// Created by Hedzr Yeh on 2021/2/10.
//


#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>


#include "version.h"
#include <cmdr-cxx.hh>

#include "second_src.hh"


// int main (int argc, char * argv[]) {
//     return Catch::Session().run( argc, argv );
// }

//


TEST_CASE("flags test", "[flags]") {

    auto cli = cmdr::app::create("flags", xVERSION_STRING, "hedzr",
                                 "Copyright © 2021 by hedzr, All Rights Reserved.",
                                 "A demo app for cmdr-c11 library.",
                                 "$ ~ --help");

    try {
        using namespace cmdr::opt;

        // cli.opt(opt_dummy<support_types>{}());

        add_server_menu(cli);
        add_test_menu(cli);
        add_main_menu(cli);

        auto &cc = cli("server");
        CMDR_ASSERT(cc.valid());
        CMDR_ASSERT(cc["count"].valid());
        CMDR_ASSERT(cc["host"].valid());
        CMDR_ASSERT(cc("status").valid());
        CMDR_ASSERT(cc("start").valid());
        CMDR_ASSERT(cc("run", true).valid());

    } catch (std::exception &e) {
        std::cerr << "Exception caught for testing (NOT BUG) : " << e.what() << '\n';
        CMDR_DUMP_STACK_TRACE(e);
    }


    SECTION("--help") {
        const char *argv[] = {"", "--help", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("help").as<bool>());
    }
    SECTION("-h") {
        const char *argv[] = {"", "-h", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("help").as<bool>());
    }

    SECTION("--version") {
        const char *argv[] = {"", "--version", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("version").as<bool>());
    }
    SECTION("-V") {
        const char *argv[] = {"", "-V", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("version").as<bool>());
    }
    SECTION("--ver") {
        const char *argv[] = {"", "--ver", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("version").as<bool>());
    }


    SECTION("--verbose") {
        const char *argv[] = {"", "--verbose", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("verbose").as<bool>());
        REQUIRE(cli.get_for_cli("no-color").as<bool>());
    }
    SECTION("-v") {
        const char *argv[] = {"", "-v", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("verbose").as<bool>());
        REQUIRE(cli.get_for_cli("no-color").as<bool>());
        REQUIRE(cli["verbose"].hit_count() == 1);
    }
    SECTION("-vvv") {
        const char *argv[] = {"", "-vvv", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("verbose").as<bool>());
        REQUIRE(cli.get_for_cli("no-color").as<bool>());
        REQUIRE(cli["verbose"].hit_count() == 3);
    }


    SECTION("--debug") {
        const char *argv[] = {"", "--debug", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("debug").as<bool>());
        REQUIRE(cli.get_for_cli("no-color").as<bool>());
    }
    SECTION("-D") {
        const char *argv[] = {"", "-D", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("debug").as<bool>());
        REQUIRE(cli.get_for_cli("no-color").as<bool>());
    }

    SECTION("~~debug") {
        const char *argv[] = {"", "~~debug", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("debug").as<bool>());
        REQUIRE(cli.get_for_cli("no-color").as<bool>());
        REQUIRE(cli["debug"].hit_special());
        REQUIRE(cli["debug"].hit_long());
        REQUIRE(cli["debug"].hit_count() == 1);
    }

    SECTION("~~tree") {
        const char *argv[] = {"", "~~tree", "--no-color"};
        REQUIRE(cli.run(countof(argv), const_cast<char **>(argv)) == 0);
        REQUIRE(cli.get_for_cli("tree").as<bool>());
        REQUIRE(cli.get_for_cli("no-color").as<bool>());
        REQUIRE(cli["tree"].hit_special());
        REQUIRE(cli["tree"].hit_long());
        REQUIRE(cli["tree"].hit_count() == 1);
    }
}
