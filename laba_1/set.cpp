#include <set.h>
#include <algorithm>
#include <vector>

Set::Set(){ }

Set::Set(const Set& other): elements(other.elements), subsets(other.subsets){

}

Set::~Set(){ }

size_t Set::power() const {
    return elements.size() + subsets.size();
}

void Set::add(const std::string& el){
    if(std::count(elements.begin(), elements.end(), el) == 0){
        elements.push_back(el);
    }
}

void Set::remove(const std::string& el){
    if(std::count(elements.begin(), elements.end(), el) > 0){
        std::erase(elements, el);
    }
}

bool Set::isEmpty() const{
    return elements.empty() && subsets.empty()
}

