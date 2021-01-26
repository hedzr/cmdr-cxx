//
// Created by Hedzr Yeh on 2021/1/21.
//

#include "cmdr11/cmdr_chrono.hh"

#include <thread>

void test_high_resolution_duration() {
    auto then = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now - then;

    auto clean_duration = cmdr::chrono::break_down_durations<std::chrono::seconds, std::chrono::milliseconds, std::chrono::microseconds>(duration);
    //    auto timeInMicroSec = std::chrono::duration_cast<std::chrono::microseconds>(duration); // base in Microsec.
    std::cout << std::get<0>(clean_duration).count() << "::" << std::get<1>(clean_duration).count() << "::" << std::get<2>(clean_duration).count() << "\n";
}

int main() {
    cmdr::chrono::high_res_duration _hrd;

    test_high_resolution_duration();
}