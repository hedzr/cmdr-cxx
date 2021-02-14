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


#define ENABLE_VARIABLE
#include "cmdr11/cmdr11.hh"

#include "custom_iterator.hh"


void small_cases() {
    cmdr::chrono::high_res_duration _timer;

    std::cout << "small_cases ------------" << '\n';

    // std::cout << "test_literals ------------" << '\n';
    //
    // extern void test_literals();
    // test_literals();
    //
    // std::cout << "test_null_stream ------------" << '\n';
    //
    // extern void test_null_stream();
    // test_null_stream();
    //
    // std::cout << "test_inplace_and_emplace ------------" << '\n';
    //
    // extern void test_inplace_and_emplace();
    // test_inplace_and_emplace();

    std::cout << "others ------------" << '\n';

    // cmdr::vars::variable::test();

    cmdr::util::fixed_array<double>::main();
}
