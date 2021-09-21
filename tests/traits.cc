//
// Created by Hedzr Yeh on 2021/9/21.
//

#include <any>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <new>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>


#include "cmdr11/cmdr_defs.hh"
#include "cmdr11/cmdr_utils.hh"
#include "cmdr11/cmdr_var_t.hh"

#include "cmdr11/cmdr_x_test.hh"

void test_tuple_to_string() {
    auto tup = std::make_tuple(1, "hello", 4.5);
    std::cout << tup << '\n';
}

void test_vector_to_string() {
    auto vec = {"a", "b", "v"};
    std::cout << vec << '\n';
}

int main() {
    CMDR_TEST_FOR(test_tuple_to_string);
    CMDR_TEST_FOR(test_vector_to_string);
}