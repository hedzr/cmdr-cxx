//
// Created by Hedzr Yeh on 2021/1/17.
//

#include <any>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <new>
#include <string>


std::size_t allocated = 0;

void *operator new(size_t sz) {
    void *p = std::malloc(sz);
    allocated += sz;
    return p;
}

void operator delete(void *p) noexcept {
    return std::free(p);
}

int main() {
    std::cout << std::boolalpha;

    allocated = 0;
    std::string s("hi");
    std::printf("stack space = %zu, heap space = %zu, capacity = %zu\n",
                sizeof(s), allocated, s.capacity());

}