#pragma once
 
#include <ostream>
 
 
template <class T>
struct vertex_traits {
    using value_type = T;
    static bool equal(const T& a, const T& b) { return a == b; }
    static bool less(const T& a, const T& b) { return a < b; }
    static void print(std::ostream& os, const T& v) { os << v; }
};
 
 