//
// Created by Hedzr Yeh on 2020/11/19.
//

#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <new>
#include <string>
#include <vector>

#include "cmdr11/cmdr_if.hh"
#include "cmdr11/cmdr_var_t.hh"

#if JUST_FOR_REFERRING
int main(int, char **) {
  std::cout << "Running tests...\n";

  std::string input = "tests and strings";
  std::string cstr  = compress_string(input);
  if (cstr != "bad one") {
    return 1;
  }

  std::string dstr = decompress_string(cstr);

  return dstr == cstr ? 0 : 1;
}
#endif

void test_types_check_1() {
  std::cout << std::boolalpha;

  std::cout << "cmdr::traits::is_vector_t: \n";
  std::cout << std::boolalpha << cmdr::traits::is_vector_v<std::vector<int>> << std::endl;
  std::cout << std::boolalpha << cmdr::traits::is_vector_v<int> << std::endl;

  std::cout << "cmdr::traits::is_container: \n";
  std::cout << std::boolalpha << cmdr::traits::is_container<std::string>::value << std::endl;
  std::cout << std::boolalpha << cmdr::traits::is_container<std::list<std::string>>::value << std::endl;

  using namespace std::chrono_literals;

  auto v1 = std::vector{"example.com", "example.org"};
  std::cout << "cmdr::traits::is_stl_container : " << cmdr::traits::is_stl_container<decltype(v1)>::value << std::endl;
  auto v2 = 10s;
  std::cout << "type " << typeid(v2).name() << ". check: " << cmdr::traits::is_duration<decltype(v2)>::value << std::endl;

  std::cout << "cmdr::traits::is_duration<std::chrono::seconds> : " << cmdr::traits::is_duration<std::chrono::seconds>::value << std::endl;
  std::cout << "cmdr::traits::is_duration<std::chrono::hours> : " << cmdr::traits::is_duration<std::chrono::hours>::value << std::endl;
  std::cout << "cmdr::traits::is_duration<std::chrono::microseconds> : " << cmdr::traits::is_duration<std::chrono::microseconds>::value << std::endl;
  std::cout << "cmdr::traits::is_duration<std::chrono::nanoseconds> : " << cmdr::traits::is_duration<std::chrono::nanoseconds>::value << std::endl;
  std::cout << "cmdr::traits::is_duration<long long> : " << cmdr::traits::is_duration<decltype(0xfull)>::value << std::endl;
  std::cout << "cmdr::traits::is_duration<bool> : " << cmdr::traits::is_duration<bool>::value << std::endl;
}

void test_var_t_1() {
  // cmdr::vars::var_t<std::string> vs;
  // std::cout << "var_t: " << vs << std::endl;
}

int main(int, char **) {
  std::cout << "Running tests..." << std::endl;

  //    std::string input = "tests and strings";
  //    std::string cstr = compress_string(input);
  //
  //    //    if (cstr != "bad one") {
  //    //        std::cerr << "PRB\n";
  //    //        return 1;
  //    //    }
  //
  //    std::string dstr = decompress_string(cstr);

  auto *v = new std::vector<int>();
  v->push_back(1);
  v->push_back(2);
  v->push_back(3);
  v->push_back(4);

  //    int ret = dstr == input ? 0 : 1;
  int ret = v->size() == 4 ? 0 : 1;
  std::cout << "return " << ret << ", " << v->size() << std::endl;


  test_types_check_1();

  test_var_t_1();

  extern void test_store_1();
  test_store_1();

  return ret;
}