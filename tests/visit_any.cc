//
// Created by Hedzr Yeh on 2021/1/20.
//


#include "visit_any.hh"

#include "cmdr11/cmdr_ios.hh"
#include "cmdr11/cmdr_utils.hh"

void test_any() {
  std::any o1{std::string("a string")};
  std::cout << std::any_cast<std::string>(o1) << '\n';
  // std::visit([](auto &&arg) { std::cout << arg; }, o1);
  std::cout << '\n';

  std::any any{-1LL};
  try {
    cmdr::util::visit_any_as<std::string, int, double, char>(any, [](auto const &x) {
      std::cout << x << '\n';
    });
  } catch (std::exception const &e) {
    std::cout << e.what() << '\n';
  }
}

void test_streamer_any() {
  std::vector<std::any> va{
      {},
      42,
      123u,
      3.14159f,
      2.71828,
      "C++17",
  };

  study::streamer_any os;

  std::cout << "{ ";
  for (const std::any &a: va) {
    os.process(std::cout, a);
    std::cout << ", ";
  }
  std::cout << "}\n";

  os.process(std::cout, std::any(0xFULL)); //< Unregistered type "y" (unsigned long long)
  std::cout << '\n';

  os.register_any_visitor<unsigned long long>([](std::ostream &os1, auto x) {
    cmdr::io::ios_state_saver _saver(os1);
    os1 << std::hex << std::showbase << x;
  });

  std::cout << std::dec << 13 << '\n';

  os.process(std::cout, std::any(0xFULL)); //< OK: 0xf
  std::cout << '\n';

  using namespace std::chrono_literals;

  os.process(std::cout, std::any(31s)); //< OK: 0xf
  std::cout << '\n';
}

auto main() -> int {
  test_any();

  test_streamer_any();

  extern void second_test();
  second_test();
}