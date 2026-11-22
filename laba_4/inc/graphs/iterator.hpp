#pragma once
 
#include <cstddef>
#include <iterator>
 
 
template <class EdgeIt>
class adjacent_iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename std::iterator_traits<EdgeIt>::value_type::second_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;
 
    adjacent_iterator() = default;
    explicit adjacent_iterator(EdgeIt it) : it_(it) {}
 
    reference operator*() const { return it_->second; }
    pointer operator->() const { return &it_->second; }
 
    adjacent_iterator& operator++() { ++it_; return *this; }
    adjacent_iterator operator++(int) { auto t = *this; ++it_; return t; }
    adjacent_iterator& operator--() { --it_; return *this; }
    adjacent_iterator operator--(int) { auto t = *this; --it_; return t; }
 
    friend bool operator==(const adjacent_iterator& a, const adjacent_iterator& b) {
        return a.it_ == b.it_;
    }
    friend bool operator!=(const adjacent_iterator& a, const adjacent_iterator& b) {
        return !(a == b);
    }
 
private:
    EdgeIt it_{};
};
 
 