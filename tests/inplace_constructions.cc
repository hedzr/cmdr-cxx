//
// Created by Hedzr Yeh on 2021/1/21.
//

#include <any>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <variant>

#include "cmdr11/cmdr_var_t.hh"

void test_inplace_and_emplace() {
    // emplace in map
    std::map<int, std::string> id_name_map;
    id_name_map.insert({10, std::string("name 1")});
    id_name_map.emplace(11, std::string("name 2"));
    id_name_map.emplace(std::piecewise_construct, std::forward_as_tuple(12), std::forward_as_tuple("name 3"));
    for (const auto &it : id_name_map) {
        std::cout << it.first << ':' << it.second << std::endl;
    }

    // emplace in optional
    std::optional<std::string> o2(std::in_place, "a string");
    std::cout << o2.value() << std::endl;
    o2.emplace(5, ' ');
    std::cout << o2.value() << std::endl;
    o2.emplace({'s', 'a', 'b'});
    std::cout << o2.value() << std::endl;

#if defined(VERBOSE_DEBUG)
    std::optional<std::string> oo(std::in_place, "a string");
    std::cout << oo.value() << std::endl;
#endif

    // inplace construction in optional
    std::optional<std::string> o3(std::in_place, 8, 'c');
    std::cout << o3.value() << std::endl;
}

void test_variant() {
    std::variant<float, long, double> z = 0;
    z = 0.0;
}

void test_any() {
    std::any z = 0;
    z = 0.0;
    z = "str";
}

void test_streamable_any() {
#if NOT_YET
    cmdr::opt::vars::var_t<cmdr::opt::vars::streamable_any> v("yes");
    std::cout << v << '\n';

    v = 123;
    std::cout << v << '\n';
#endif
}

int main() {
    test_variant();
    test_any();
    test_inplace_and_emplace();
    test_streamable_any();
}