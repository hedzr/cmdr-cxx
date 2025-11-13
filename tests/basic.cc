// cmdr-cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/25.
//

#if (__cplusplus > 202002L) // c++23 and higher

// import std;  // not yet

#include <cstdlib> // for EXIT_FAILURE, ...
#include <print>   // this form should work down to c++17+
// #include <iomanip>
// #include <iostream>

#include "cmdr-cxx.hh"

auto main(int argc, char *argv[]) -> int {
  cmdr::test::test_for_macros(); // internal tests

  // std::println("Hello, World!\n");
  std::print("Hello, {}! __cplusplus = {}.\n", "World", __cplusplus); // outputs: Hello, World!
  std::print("The answer is: {}\n", 42);                              // outputs: The answer is: 42

  if (argc < 1) {
    fprintf(stderr, "err: we need any arguments passed in.\n");
    return EXIT_FAILURE; // = 1
  }

  UNUSED(argv);
  return EXIT_SUCCESS; // = 0
}

#else // below c++23
#include "cmdr-cxx.hh"
#include <iomanip>
#include <iostream>
auto main() -> int { std::cout << "Hello, World! " << __cplusplus << '\n'; }
#endif