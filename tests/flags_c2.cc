//
// Created by Hedzr Yeh on 2021/2/10.
//


#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>


#include <cmdr-cxx.hh>

#include "second_src.hh"


// int main (int argc, char * argv[]) {
//     return Catch::Session().run( argc, argv );
// }

//


TEST_CASE("flags test", "[flags]") {

  auto &cli = cmdr::cli("flags", CMDR_VERSION_STRING, "hedzr",
                        "Copyright © 2021 by hedzr, All Rights Reserved.",
                        "A demo app for cmdr-c11 library.",
                        "$ ~ --help");

  try {
    using namespace cmdr::opt;

    // cli.opt(opt_dummy<support_types>{}());

    cli.reset();

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
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("help").as<bool>());
  }
  SECTION("-h") {
    const char *argv[] = {"", "-h", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("help").as<bool>());
  }
  SECTION("-h-v") {
    const char *argv[] = {"", "-h-v", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("help").as<bool>() == false);
    REQUIRE(cli.get_for_cli("verbose").as<bool>());
  }

  SECTION("--version") {
    const char *argv[] = {"", "--version", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("version").as<bool>());
  }
  SECTION("-V") {
    const char *argv[] = {"", "-V", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("version").as<bool>());
  }
  SECTION("--ver") {
    const char *argv[] = {"", "--ver", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("version").as<bool>());
  }


  SECTION("--verbose") {
    const char *argv[] = {"", "--verbose", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("verbose").as<bool>());
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
  }
  SECTION("-v") {
    const char *argv[] = {"", "-v", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("verbose").as<bool>());
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
    REQUIRE(cli["verbose"].hit_count() == 1);
  }
  SECTION("-vvv") {
    const char *argv[] = {"", "-vvv", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("verbose").as<bool>());
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
    REQUIRE(cli["verbose"].hit_count() == 3);
  }


  SECTION("--debug") {
    const char *argv[] = {"", "--debug", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("debug").as<bool>());
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
  }
  SECTION("-D") {
    const char *argv[] = {"", "-D", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("debug").as<bool>());
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
  }

  SECTION("~~debug") {
    const char *argv[] = {"", "~~debug", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("debug").as<bool>());
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
    REQUIRE(cli["debug"].hit_special());
    REQUIRE(cli["debug"].hit_long());
    REQUIRE(cli["debug"].hit_count() == 1);
  }

  SECTION("~~tree") {
    const char *argv[] = {"", "~~tree", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("tree").as<bool>());
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
    REQUIRE(cli["tree"].hit_special());
    REQUIRE(cli["tree"].hit_long());
    REQUIRE(cli["tree"].hit_count() == 1);
  }

  // default value of an arg/flag, ...

  SECTION("default value of an arg/flag: main --float 2.7") {
    const char *argv[] = {"", "main", "--float", "2.7", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);

    REQUIRE(cli.get_for_cli("main.int").as<int>() == 10);
    REQUIRE(cli.get_for_cli("main.long").as<long>() == 79129L);
    REQUIRE(cli.get_for_cli("main.float").as<float>() == 2.7f);
    REQUIRE(cli.get_for_cli("main.long-long").as<long long>() == 98LL);
    auto t1 = cli.get_for_cli("main.string-array");
    std::cout << "cli.get_for_cli(\"main.string-array\"): " << t1 << '\n';
    auto v1 = t1.as<std::vector<const char *>>();
    auto v2 = std::vector{"a", "Z"};
    REQUIRE(cmdr::util::compare_vector_values(v1, v2));
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
  }

  // store

  SECTION("store: main --float 2.7") {
    cmdr::set("wudao.count", 1);
    cmdr::set("wudao.string", "str");
    cmdr::set("wudao.float", 3.14f);
    cmdr::set("wudao.double", 2.718);
    cmdr::set("wudao.array", std::vector{"a", "b", "c"});
    cmdr::set("wudao.bool", false);

    std::cout << cmdr::get<int>("wudao.count") << '\n';
    auto const &aa = cmdr::get<std::vector<char const *>>("wudao.array");
    std::cout << cmdr::string::join(aa, ", ", "[", "]") << '\n';
    cmdr::vars::variable &ab = cmdr::get_app().get("wudao.array");
    std::cout << ab << '\n';

    const char *argv[] = {"", "main", "--version", "--no-color"};
    REQUIRE(cli.run((int) countof(argv), const_cast<char **>(argv)) == 0);
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
  }

  // required flag

  SECTION("required flag: main sub1 --float 2.7") {
    const char *argv[] = {"", "main", "sub1", "--float", "2.7", "--no-color"};
    try {
      cli.set_no_catch_cmdr_biz_error(true).run((int) countof(argv), const_cast<char **>(argv));
    } catch (cmdr::exception::cmdr_biz_error const &e) {
      auto c{cmdr::terminal::colors::colorize::create()};
      std::cout << c.bold().s("<<CAPTURED>> ") << e.what() << '\n';
    }
    REQUIRE(cli.get_for_cli("main.long").as<long>() == 79129L);
    REQUIRE(cli.get_for_cli("main.float").as<float>() == 2.7f);
    REQUIRE(cli.get_for_cli("no-color").as<bool>());
  }
}
