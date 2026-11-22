#pragma once
 
#include <cstddef>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

template <class It, class Compare = std::less<>>
void binary_tree_sort(It first, It last, Compare comp = Compare()) {
    using value_type = typename std::iterator_traits<It>::value_type;
    constexpr std::size_t nil = static_cast<std::size_t>(-1);
 
    struct Node {
        value_type value;
        std::size_t left;
        std::size_t right;
    };
    std::vector<Node> nodes;

    for (It it = first; it != last; ++it) {
        const std::size_t idx = nodes.size();
        nodes.push_back(Node{std::move(*it), nil, nil});
        if (idx == 0) continue;
 
        std::size_t cur = 0;
        for (;;) {
            std::size_t& next = comp(nodes[idx].value, nodes[cur].value)
                                    ? nodes[cur].left
                                    : nodes[cur].right;
            if (next == nil) {
                next = idx;
                break;
            }
            cur = next;
        }
    }

    std::vector<std::size_t> stack;
    std::size_t cur = nodes.empty() ? nil : 0;
    It out = first;
    while (cur != nil || !stack.empty()) {
        while (cur != nil) {
            stack.push_back(cur);
            cur = nodes[cur].left;
        }
        cur = stack.back();
        stack.pop_back();
        *out = std::move(nodes[cur].value);
        ++out;
        cur = nodes[cur].right;
    }
}

template <class T, std::size_t N, class Compare = std::less<>>
void binary_tree_sort(T (&arr)[N], Compare comp = Compare()) {
    binary_tree_sort(arr, arr + N, comp);
}
 
template <class T, class Alloc, class Compare = std::less<>>
void binary_tree_sort(std::vector<T, Alloc>& v, Compare comp = Compare()) {
    binary_tree_sort(v.begin(), v.end(), comp);
}