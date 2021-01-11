//
// Created by Hedzr Yeh on 2021/1/21.
//

#include <chrono>
#include <iomanip>
#include <iostream>

void test_literals() {
    using namespace std::chrono_literals;

    auto lesson = 45min;
    auto half_min = 0.5min;
    std::cout << "one lesson is " << lesson.count() << " minutes\n"
              << "half a minute is " << half_min.count() << " minutes\n";

    auto d1 = 250ms;
    std::chrono::milliseconds d2 = 1s;
    std::cout << "250ms = " << d1.count() << " milliseconds\n"
              << "1s = " << d2.count() << " milliseconds\n";

    // using namespace std::literals;
    // std::cout
    //         << int(2020y) << ' '
    //         << int(-220y) << ' '
    //         << int(3000y) << ' '
    //         << int(32768y) << ' '    // 未指定
    //         << int(65578y) << '\n';  // 未指定
}

int main() {
    test_literals();
}