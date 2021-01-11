//
// Created by Hedzr Yeh on 2021/1/13.
//

#include <any>
#include <optional>
#include <variant>

#include <algorithm>
#include <functional>
#include <utility>

#include <cassert>
#include <stdexcept>

#include <iomanip>
#include <iostream>
#include <sstream>

#include <string>

#include <type_traits>
#include <typeindex>
#include <typeinfo>

#include <initializer_list>

#include <chrono>


#include "cmdr11/cmdr11.hh"
#include "version.h"


#include "custom_iterator.hh"


void small_cases() {
    cmdr::chrono::high_res_duration _timer;

    std::cout << "small_cases ------------" << std::endl;

    // std::cout << "test_literals ------------" << std::endl;
    //
    // extern void test_literals();
    // test_literals();
    //
    // std::cout << "test_null_stream ------------" << std::endl;
    //
    // extern void test_null_stream();
    // test_null_stream();
    //
    // std::cout << "test_inplace_and_emplace ------------" << std::endl;
    //
    // extern void test_inplace_and_emplace();
    // test_inplace_and_emplace();

    std::cout << "others ------------" << std::endl;

    cmdr::opt::vars::variable<int>::test();

    cmdr::util::fixed_array<double>::main();
}
