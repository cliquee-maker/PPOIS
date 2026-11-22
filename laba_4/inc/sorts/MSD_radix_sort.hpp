
#pragma once
 
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>
 
struct default_radix_key {
    template <class T>
    auto operator()(const T& v) const {
        static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>,
                      "default_radix_key: нужен целочисленный тип, "
                      "для остальных типов передайте свой извлекатель ключа");
        using U = std::make_unsigned_t<T>;
        U u = static_cast<U>(v);
        if constexpr (std::is_signed_v<T>) {
            u = static_cast<U>(u ^ static_cast<U>(U(1) << (sizeof(U) * 8 - 1)));
        }
        return u;
    }
};
 
namespace detail {
 
template <class It, class ValuePtr, class Key>
void msd_radix_impl(It first, It last, ValuePtr buf, std::size_t byte, Key& key) {
    const auto n = static_cast<std::size_t>(std::distance(first, last));
    if (n < 2) return;
 
    if (n <= 16) {
        std::stable_sort(first, last, [&](const auto& a, const auto& b) {
            return key(a) < key(b);
        });
        return;
    }
 
    const unsigned shift = static_cast<unsigned>(byte * 8);
    auto digit = [&](const auto& v) {
        return static_cast<std::size_t>((key(v) >> shift) & 0xFFu);
    };
 
    std::size_t count[257] = {};
    for (It it = first; it != last; ++it) ++count[digit(*it) + 1];
    for (std::size_t i = 1; i < 257; ++i) count[i] += count[i - 1];
 
    std::size_t pos[256];
    std::copy(count, count + 256, pos);
    for (It it = first; it != last; ++it) buf[pos[digit(*it)]++] = std::move(*it);
    std::move(buf, buf + n, first);
 
    if (byte == 0) return;
    for (std::size_t b = 0; b < 256; ++b) {
        const std::size_t lo = count[b], hi = count[b + 1];
        if (hi - lo > 1) {
            msd_radix_impl(std::next(first, static_cast<std::ptrdiff_t>(lo)),
                           std::next(first, static_cast<std::ptrdiff_t>(hi)),
                           buf + lo, byte - 1, key);
        }
    }
}
 
}
 
template <class It, class KeyFn = default_radix_key>
void msd_radix_sort(It first, It last, KeyFn key = KeyFn()) {
    using value_type = typename std::iterator_traits<It>::value_type;
    using key_type = std::decay_t<decltype(key(*first))>;
    static_assert(std::is_integral_v<key_type> && std::is_unsigned_v<key_type> &&
                      !std::is_same_v<key_type, bool>,
                  "msd_radix_sort: ключ должен быть беззнаковым целым");
 
    if (first == last) return;
    std::vector<value_type> buffer(static_cast<std::size_t>(std::distance(first, last)));
    detail::msd_radix_impl(first, last, buffer.data(), sizeof(key_type) - 1, key);
}
 
template <class T, std::size_t N, class KeyFn = default_radix_key>
void msd_radix_sort(T (&arr)[N], KeyFn key = KeyFn()) {
    msd_radix_sort(arr, arr + N, key);
}
 
template <class T, class Alloc, class KeyFn = default_radix_key>
void msd_radix_sort(std::vector<T, Alloc>& v, KeyFn key = KeyFn()) {
    msd_radix_sort(v.begin(), v.end(), key);
}
 
 