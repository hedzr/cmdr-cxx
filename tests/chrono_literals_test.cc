//
// Created by Hedzr Yeh on 2021/1/21.
//

#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

#include "cmdr11/cmdr_chrono.hh"


void test_literals() {
  using namespace std::chrono_literals;

  auto lesson   = 45min;
  auto half_min = 0.5min;
  std::cout << "one lesson is " << lesson.count() << " minutes\n"
            << "half a minute is " << half_min.count() << " minutes\n";

  auto d1                      = 250ms;
  std::chrono::milliseconds d2 = 1s;
  std::cout << "250ms = " << d1.count() << " milliseconds\n"
            << "1s = " << d2.count() << " milliseconds\n";

  auto aa = std::vector<std::chrono::duration<long double, std::ratio<60>>>{
      3ns,
      800ms,
      59.739us,
      0.75min,
      501ns,
      730us,
      233ms,
      7s,
      7.2s,
      1024h,
      89.843204843s,
  };

  static_assert(cmdr::chrono::is_duration<decltype(7.5min)>::value);
  std::cout << 7.5min;

  for (auto const &vxa: aa) {
    std::cout << vxa << '\n';
  }

  std::cout << 918734032564785ns << "\n";
  std::cout << "3. " << std::setprecision(3) << 918734032564785ns << "\n";
  std::cout << std::setprecision(9) << 918734032564785ns << "\n";
  std::cout << std::setprecision(0) << 918734032564785ns << "\n";
  std::cout << std::setprecision(3) << 432034ms << "\n";
  std::cout << 14h + 32min + 37s + 645ms << "\n";
  std::cout << 86472s << "\n";
  std::cout << 4324ms << "\n";

  // using namespace std::literals;
  // std::cout
  //         << int(2020y) << ' '
  //         << int(-220y) << ' '
  //         << int(3000y) << ' '
  //         << int(32768y) << ' '    // 未指定
  //         << int(65578y) << '\n';  // 未指定
}

#if FMT_ENABLED
#include <fmt/chrono.h>

void test_fmt() {
  using namespace std::literals::chrono_literals;
  fmt::print("Default format: {} {}\n", 42s, 100ms);
  fmt::print("strftime-like format: {:%H:%M:%S}\n", 3h + 15min + 30s);
}

#else

void test_fmt() {}

#endif

int main() {
  test_literals();
  test_fmt();
}