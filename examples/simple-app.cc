// cmdr-cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/16.
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
  auto &cli = cmdr::cli("app2", CMDR_VERSION_STRING, "Sample Authors",
                        "Copyright © 2021 by Sample Authors, All Rights Reserved.",
                        "A demo app for cmdr-c11 library.",
                        "$ ~ --help");

  try {
    // using namespace cmdr::opt;
    add_test_menu(cli);
  } catch (std::exception &e) {
    std::cerr << "Exception caught for testing (NOT BUG) : " << e.what() << '\n';
    CMDR_DUMP_STACK_TRACE(e);
  }

  return cli.run(argc, argv);
}

void add_test_menu(cmdr::app &cli) {
  using namespace cmdr::opt;

  cli += sub_cmd{}("test", "t", "test-command")
         .description("main tests commands for testing")
         .group("Test"); {
    auto &t1 = *(cli.last_added_command());
    t1 += sub_cmd{}("hello", "hi")
          .description("hello world in cmdr-cxx subcmd way")
          .group("Test")
          .on_invoke([](cmd const &cc, string_array const &) -> int {
            std::cout << "Hello, World!\n";

            // auto &s = cli.store();
            auto &s = cmdr::get_store();

            // get the final value of a option object, directly
            const auto &k = s.get_raw_p(DEFAULT_CLI_KEY_PREFIX, "test.int");
            if (k.has_value())
              std::cout << "int.val:  " << k.as_string() << '\n';

            // the recommendatory way is getting the option's node in the app-store at first,
            auto sc = s.subtree(DEFAULT_CLI_KEY_PREFIX);
            assert(sc->is_null() == false);
            std::cout << "string:   " << sc->get_raw("test.string").as_string() << '\n';
            std::cout << "float:    " << sc->get_raw("test.float").as_string() << '\n';
            std::cout << "double:   " << sc->get_raw("test.double").as_string() << '\n';
            std::cout << '\n' << "int:      " << cc["int"].default_value() << '\n';
            return 0;
          });
    t1 += opt{10}("int", "i")
        .description("set the int-value");
    t1 += opt{""}("string", "str")
        .description("set the string-value");
    t1 += opt{3.13f}("float", "f")
        .description("flag float");
    t1 += opt{3.12}("double", "d")
        .description("flag double");
  }
}
