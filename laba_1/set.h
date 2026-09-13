#ifndef SET
#define SET

#include <iostream>
#include <vector>
#include <string>

class Set{
    std::vector<std::string> elements;
    std::vector<Set> subsets;
public:
    set();
    ~set();

    set(const Set& other);
    void add(const std::string& element);
    void remove(const std::string& element);
    const bool isEmpty();
    const size_t power();
}

#endif