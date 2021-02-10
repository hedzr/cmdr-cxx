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
std::size_t released = 0;
std::size_t released_size = 0;

void *operator new(size_t sz) {
    void *p = std::malloc(sz);
    allocated += sz;
    return p;
}

void operator delete(void *p) noexcept {
    released++;
    return std::free(p);
}

void operator delete(void *p, std::size_t sz) noexcept {
    released++;
    released_size += sz;
    return std::free(p);
}

int main() {
    std::cout << std::boolalpha;

    std::string s("hi");
    std::printf("stack space = %zu, heap space = %zu (released: %zu), capacity = %zu\n",
                sizeof(s), allocated, released, s.capacity());

    auto re1 = new std::string("moli");
    delete re1;
    std::printf("stack space = %zu, heap space = %zu (released: %zu, %zu), capacity = %zu\n",
                sizeof(s), allocated, released, released_size, s.capacity());
}