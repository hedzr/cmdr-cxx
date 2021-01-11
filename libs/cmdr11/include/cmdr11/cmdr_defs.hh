//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_DEFS_HH
#define CMDR_CXX11_CMDR_DEFS_HH


#include <initializer_list>
#include <iostream>
#include <utility>
#include <vector>


#if !defined(_DEBUG) && defined(DEBUG)
#define _DEBUG DEBUG
#endif

#ifndef unused
#define unused(x) (void) (x)
#endif

#ifndef UNUSED
#define UNUSED(x) (void) (x)
#endif

#ifndef __COPY
#define __COPY(m) this->m = o.m
#endif

template<typename T>
struct always_false : std::false_type {
};

template<typename T>
constexpr bool always_false_v = always_false<T>::value;


typedef const char *const_chars;
typedef std::vector<std::string> string_array;


const char *const UNSORTED_GROUP = "1230.Unsorted";
const char *const SYS_MGMT_GROUP = "9000.System Management";

//template<class T=std::string>
//constexpr T UNSORTED_GROUP = T("1230.Unsorted");
//
//template<class T=std::string>
//constexpr T SYS_MGMT_GROUP = T("9000.System Management");


#endif //CMDR_CXX11_CMDR_DEFS_HH
