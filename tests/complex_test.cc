//
// Created by Hedzr Yeh on 2021/1/29.
//

#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>

int main() {
    using namespace std::complex_literals;
    std::cout << std::fixed << std::setprecision(1);

    std::complex<double> z1 = 1i * 1i; // 虚数单位平方
    std::cout << "i * i = " << z1 << '\n';

    std::complex<double> z2 = std::pow(1i, 2); // 虚数单位平方
    std::cout << "pow(i, 2) = " << z2 << '\n';

    double PI = std::acos(-1);
    std::complex<double> z3 = std::exp(1i * PI); // 欧拉方程
    std::cout << "exp(i * pi) = " << z3 << '\n';

    std::complex<double> z4 = 1. + 2i, z5 = 1. - 2i; // 共轭
    std::cout << "(1+2i)*(1-2i) = " << z4 * z5 << '\n';
}