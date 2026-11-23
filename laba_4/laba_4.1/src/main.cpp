#include "../inc/Sorting.h"
#include "../inc/Schoolboy.h"
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

template <typename Container>
void print(const std::string& title, const Container& container) {
    std::cout << title << ":";
    for (const auto& value : container) {
        std::cout << " " << value;
    }
    std::cout << '\n';
}

void printArray(const std::string& title, const int* values, std::size_t size) {
    std::cout << title << ":";
    for (std::size_t index = 0; index < size; ++index) {
        std::cout << " " << values[index];
    }
    std::cout << '\n';
}

}  

int main() {
    int numbers[] = {9, 1, 5, 3, 7, 3, 8, 2, 6, 4, 0};
    printArray("array before", numbers, std::size(numbers));
    sorting::binaryTreeSort(numbers);
    printArray("array after binary tree sort", numbers, std::size(numbers));

    int radixNumbers[] = {170, 45, 75, 90, 802, 24, 2, 66};
    printArray("radix array before", radixNumbers, std::size(radixNumbers));
    sorting::msdRadixSort(radixNumbers);
    printArray("radix array after ", radixNumbers, std::size(radixNumbers));

    std::vector<double> values{3.5, -1.25, 8.0, 0.0, 2.75, -4.5};
    print("vector before        ", values);
    sorting::binaryTreeSort(values, std::greater<double>{});
    print("vector descending    ", values);

    std::array<std::string, 5> words{"pear", "apple", "fig", "banana", "kiwi"};
    print("strings before       ", words);
    sorting::binaryTreeSort(words);
    print("strings after        ", words);

    std::vector<sorting::Schoolboy> schoolboys{
        {"Alice", 5, 4.7}, {"Bob", 3, 3.4},  {"Carol", 7, 4.9},
        {"Dave", 2, 4.2},  {"Eve", 5, 4.7},   {"Frank", 9, 3.9}};

    print("schoolboys before           ", schoolboys);

    sorting::binaryTreeSort(schoolboys);
    print("schoolboys by avg (tree)    ", schoolboys);

    sorting::binaryTreeSort(
        schoolboys, [](const sorting::Schoolboy& left,
                       const sorting::Schoolboy& right) {
            return left.name() > right.name();
        });
    print("schoolboys by name desc     ", schoolboys);

    sorting::msdRadixSortByKey(
        schoolboys, [](const sorting::Schoolboy& s) { return s.grade(); });
    print("schoolboys by grade (radix) ", schoolboys);

    sorting::msdRadixSortByKey(
        schoolboys, [](const sorting::Schoolboy& s) {
            return static_cast<int>(s.averageScore() * 100.0 + 0.5);
        });
    print("schoolboys by avg (radix)   ", schoolboys);

    std::vector<int> ints{170, 45, 75, 90, 802, 24, 2, 66};
    print("ints before msd      ", ints);
    sorting::msdRadixSort(ints);
    print("ints after msd       ", ints);

    std::vector<std::string> manyWords{
        "pear", "apple", "fig", "banana", "kiwi", "apricot", "blueberry"};
    print("words before msd     ", manyWords);
    sorting::msdRadixSort(manyWords);
    print("words after msd      ", manyWords);

    std::cout << "array sorted: "
              << std::boolalpha
              << sorting::isSorted(std::begin(numbers), std::end(numbers))
              << '\n';

    return 0;
}