#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace sorting {

template <typename RandomIt, typename Compare>
void binaryTreeSort(RandomIt first, RandomIt last, Compare comp) {
    using Value = typename std::iterator_traits<RandomIt>::value_type;
    constexpr std::size_t nil = static_cast<std::size_t>(-1);

    struct Node {
        Value value;
        std::size_t left;
        std::size_t right;
    };

    std::vector<Node> nodes;

    for (RandomIt it = first; it != last; ++it) {
        const std::size_t idx = nodes.size();
        nodes.push_back(Node{std::move(*it), nil, nil});
        if (idx == 0) {
            continue;
        }

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
    RandomIt out = first;

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

template <typename RandomIt>
void binaryTreeSort(RandomIt first, RandomIt last) {
    using Value = typename std::iterator_traits<RandomIt>::value_type;
    binaryTreeSort(first, last, std::less<Value>{});
}

template <typename T, std::size_t N, typename Compare>
void binaryTreeSort(T (&array)[N], Compare comp) {
    binaryTreeSort(std::begin(array), std::end(array), comp);
}

template <typename T, std::size_t N>
void binaryTreeSort(T (&array)[N]) {
    binaryTreeSort(std::begin(array), std::end(array));
}

template <typename Container,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void binaryTreeSort(Container& container) {
    binaryTreeSort(std::begin(container), std::end(container));
}

template <typename Container, typename Compare,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void binaryTreeSort(Container& container, Compare comp) {
    binaryTreeSort(std::begin(container), std::end(container), comp);
}

namespace detail {

template <typename T>
using unsigned_key_t = std::make_unsigned_t<T>;

template <typename T>
unsigned_key_t<T> radixKey(T value) {
    using U = unsigned_key_t<T>;
    U u = static_cast<U>(value);
    if constexpr (std::is_signed_v<T>) {
        u ^= (U(1) << (std::numeric_limits<U>::digits - 1));
    }
    return u;
}

template <typename RandomIt, typename U>
void msdRadixSortIntegralBits(RandomIt first, RandomIt last, int bit) {
    if (bit < 0 || std::distance(first, last) < 2) {
        return;
    }

    auto mid = std::stable_partition(first, last, [bit](const auto& value) {
        return (radixKey(value) & (U(1) << bit)) == 0;
    });

    msdRadixSortIntegralBits<RandomIt, U>(first, mid, bit - 1);
    msdRadixSortIntegralBits<RandomIt, U>(mid, last, bit - 1);
}

}  

template <typename RandomIt>
std::enable_if_t<
    std::is_integral_v<typename std::iterator_traits<RandomIt>::value_type> &&
    !std::is_same_v<typename std::iterator_traits<RandomIt>::value_type, bool>>
msdRadixSort(RandomIt first, RandomIt last) {
    using Value = typename std::iterator_traits<RandomIt>::value_type;
    using U = detail::unsigned_key_t<Value>;

    if (std::distance(first, last) < 2) {
        return;
    }

    detail::msdRadixSortIntegralBits<RandomIt, U>(
        first, last, std::numeric_limits<U>::digits - 1);
}

namespace detail {

inline int stringCharAt(const std::string& s, std::size_t d) {
    return d < s.size() ? static_cast<unsigned char>(s[d]) + 1 : 0;
}

inline void msdRadixSortStrings(std::vector<std::string>& a,
                                std::vector<std::string>& aux,
                                std::size_t lo,
                                std::size_t hi,
                                std::size_t d) {
    if (hi - lo < 2) {
        return;
    }

    constexpr std::size_t R = 257;  
    std::array<std::size_t, R + 1> count{};

    for (std::size_t i = lo; i < hi; ++i) {
        ++count[stringCharAt(a[i], d) + 1];
    }

    for (std::size_t r = 0; r < R; ++r) {
        count[r + 1] += count[r];
    }

    std::array<std::size_t, R + 1> start = count;
    std::array<std::size_t, R> pos{};

    for (std::size_t r = 0; r < R; ++r) {
        pos[r] = start[r];
    }

    for (std::size_t i = lo; i < hi; ++i) {
        const int c = stringCharAt(a[i], d);
        aux[pos[c]++] = std::move(a[i]);
    }

    for (std::size_t i = lo; i < hi; ++i) {
        a[i] = std::move(aux[i - lo]);
    }

    for (std::size_t r = 1; r < R; ++r) {
        const std::size_t bucketLo = lo + start[r];
        const std::size_t bucketHi = lo + start[r + 1];
        msdRadixSortStrings(a, aux, bucketLo, bucketHi, d + 1);
    }
}

}  

template <typename RandomIt>
std::enable_if_t<std::is_same_v<
    typename std::iterator_traits<RandomIt>::value_type, std::string>>
msdRadixSort(RandomIt first, RandomIt last) {
    std::vector<std::string> values(first, last);
    std::vector<std::string> aux(values.size());

    if (!values.empty()) {
        detail::msdRadixSortStrings(values, aux, 0, values.size(), 0);
    }

    std::move(values.begin(), values.end(), first);
}


namespace detail {

template <typename RandomIt, typename KeyFunc, typename U>
void msdRadixSortByKeyBits(RandomIt first, RandomIt last,
                           int bit, KeyFunc key) {
    if (bit < 0 || std::distance(first, last) < 2) {
        return;
    }

    auto mid = std::stable_partition(
        first, last, [bit, &key](const auto& value) {
            using Key = std::invoke_result_t<KeyFunc, const decltype(value)&>;
            using UK = std::make_unsigned_t<Key>;
            UK k = static_cast<UK>(key(value));
            if constexpr (std::is_signed_v<Key>) {
                k ^= (UK(1) << (std::numeric_limits<UK>::digits - 1));
            }
            return (k & (U(1) << bit)) == 0;
        });

    msdRadixSortByKeyBits<RandomIt, KeyFunc, U>(first, mid, bit - 1, key);
    msdRadixSortByKeyBits<RandomIt, KeyFunc, U>(mid, last, bit - 1, key);
}

}  

template <typename RandomIt, typename KeyFunc>
void msdRadixSortByKey(RandomIt first, RandomIt last, KeyFunc key) {
    using Value = typename std::iterator_traits<RandomIt>::value_type;
    using Key = std::invoke_result_t<KeyFunc, const Value&>;
    static_assert(std::is_integral_v<Key> && !std::is_same_v<Key, bool>,
                  "KeyFunc must return an integral non-bool type");

    using U = std::make_unsigned_t<Key>;
    if (std::distance(first, last) < 2) {
        return;
    }

    detail::msdRadixSortByKeyBits<RandomIt, KeyFunc, U>(
        first, last, std::numeric_limits<U>::digits - 1, key);
}

template <typename Container, typename KeyFunc,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void msdRadixSortByKey(Container& container, KeyFunc key) {
    msdRadixSortByKey(std::begin(container), std::end(container), key);
}

template <typename T, std::size_t N>
void msdRadixSort(T (&array)[N]) {
    msdRadixSort(std::begin(array), std::end(array));
}

template <typename Container,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void msdRadixSort(Container& container) {
    msdRadixSort(std::begin(container), std::end(container));
}

template <typename RandomIt, typename Compare>
bool isSorted(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) {
        return true;
    }

    RandomIt previous = first;
    for (RandomIt current = std::next(first); current != last;
         ++previous, ++current) {
        if (comp(*current, *previous)) {
            return false;
        }
    }
    return true;
}

template <typename RandomIt>
bool isSorted(RandomIt first, RandomIt last) {
    using Value = typename std::iterator_traits<RandomIt>::value_type;
    return isSorted(first, last, std::less<Value>{});
}

}  