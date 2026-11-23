#include <UnitTest++/UnitTest++.h>

#include "Schoolboy.h"
#include "Sorting.h"

#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <limits>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using sorting::binaryTreeSort;
using sorting::isSorted;
using sorting::msdRadixSort;
using sorting::msdRadixSortByKey;
using sorting::Schoolboy;

SUITE(BinaryTreeSortTests)
{
    TEST(EmptyRange)
    {
        std::vector<int> values;
        binaryTreeSort(values);
        CHECK(values.empty());
        CHECK(isSorted(values.begin(), values.end()));
    }

    TEST(SingleElement)
    {
        std::vector<int> values{42};
        binaryTreeSort(values);
        CHECK_EQUAL(1, static_cast<int>(values.size()));
        CHECK_EQUAL(42, values.front());
    }

    TEST(AlreadySortedAndReverseSorted)
    {
        std::vector<int> ascending{1, 2, 3, 4, 5, 6};
        binaryTreeSort(ascending);
        CHECK(ascending == std::vector<int>({1, 2, 3, 4, 5, 6}));

        std::vector<int> descending{9, 7, 5, 3, 1, -1};
        binaryTreeSort(descending);
        CHECK(descending == std::vector<int>({-1, 1, 3, 5, 7, 9}));
    }

    TEST(Duplicates)
    {
        std::vector<int> values{4, 1, 4, 2, 1, 4, 2, 1};
        binaryTreeSort(values);
        CHECK(values == std::vector<int>({1, 1, 1, 2, 2, 4, 4, 4}));
    }

    TEST(NegativeNumbers)
    {
        std::vector<int> values{-5, 0, -1, 7, -12, 3, -5};
        binaryTreeSort(values);
        CHECK(values == std::vector<int>({-12, -5, -5, -1, 0, 3, 7}));
    }

    TEST(DescendingComparator)
    {
        std::vector<double> values{3.5, -1.25, 8.0, 0.0, 2.75, -4.5};
        binaryTreeSort(values, std::greater<double>{});
        CHECK(values == std::vector<double>({8.0, 3.5, 2.75, 0.0, -1.25, -4.5}));
        CHECK(isSorted(values.begin(), values.end(), std::greater<double>{}));
    }

    TEST(RawArrayAndStdArray)
    {
        int raw[] = {5, -2, 9, 0, 4, -7};
        binaryTreeSort(raw);
        CHECK(isSorted(std::begin(raw), std::end(raw)));
        CHECK_EQUAL(-7, raw[0]);
        CHECK_EQUAL(9, raw[5]);

        std::array<int, 5> values{{10, 3, 7, 1, 3}};
        binaryTreeSort(values);
        CHECK(isSorted(values.begin(), values.end()));
        std::array<int, 5> expected{{1, 3, 3, 7, 10}};
        CHECK(values == expected);
    }

    TEST(CustomComparatorOnObjects)
    {
        std::vector<std::string> words{"pear", "apple", "fig", "banana", "kiwi"};
        binaryTreeSort(words, [](const std::string& left, const std::string& right) {
            if (left.size() != right.size()) {
                return left.size() < right.size();
            }
            return left < right;
        });

        CHECK(words == std::vector<std::string>({"fig", "kiwi", "pear", "apple", "banana"}));
    }
}

SUITE(BinaryTreeSortAdvancedTests)
{
    TEST(LargeRandomVector)
    {
        std::mt19937 generator(12345);
        std::uniform_int_distribution<int> distribution(-1000000, 1000000);

        std::vector<int> values(5000);
        for (int& value : values) {
            value = distribution(generator);
        }

        std::vector<int> expected = values;
        std::sort(expected.begin(), expected.end());

        binaryTreeSort(values);
        CHECK(values == expected);
    }

    TEST(MoveOnlyElements)
    {
        std::vector<std::unique_ptr<int>> values;
        for (int value : {5, 3, 9, 1, 7, 3}) {
            values.push_back(std::make_unique<int>(value));
        }

        binaryTreeSort(values, [](const std::unique_ptr<int>& left,
                                  const std::unique_ptr<int>& right) {
            return *left < *right;
        });

        std::vector<int> actual;
        for (const auto& value : values) {
            actual.push_back(*value);
        }
        CHECK(actual == std::vector<int>({1, 3, 3, 5, 7, 9}));
    }

    TEST(PreservesAllElements)
    {
        std::vector<std::pair<int, int>> values{
            {3, 0}, {1, 1}, {3, 2}, {2, 3}, {1, 4}, {3, 5}, {2, 6}};

        auto byKey = [](const std::pair<int, int>& left,
                        const std::pair<int, int>& right) {
            return left.first < right.first;
        };

        binaryTreeSort(values, byKey);
        CHECK(isSorted(values.begin(), values.end(), byKey));

        std::vector<int> payloads;
        for (const auto& value : values) {
            payloads.push_back(value.second);
        }
        std::sort(payloads.begin(), payloads.end());
        CHECK(payloads == std::vector<int>({0, 1, 2, 3, 4, 5, 6}));
    }

    TEST(SchoolboyDefaultOrdering)
    {
        std::vector<Schoolboy> schoolboys{
            {"Alice", 5, 4.7}, {"Bob", 3, 3.4},  {"Carol", 7, 4.9},
            {"Dave", 2, 4.2},  {"Eve", 5, 4.7}};

        binaryTreeSort(schoolboys);
        CHECK(isSorted(schoolboys.begin(), schoolboys.end()));
        CHECK(schoolboys.front().name() == "Bob");
        CHECK(schoolboys.back().name() == "Carol");
        CHECK(schoolboys[2].name() == "Alice");
        CHECK(schoolboys[3].name() == "Eve");

        binaryTreeSort(
            schoolboys, [](const Schoolboy& left, const Schoolboy& right) {
                return left.name() > right.name();
            });
        CHECK(isSorted(schoolboys.begin(), schoolboys.end(),
                       [](const Schoolboy& left, const Schoolboy& right) {
                           return left.name() > right.name();
                       }));
        CHECK(schoolboys.front().name() == "Eve");
    }
}

SUITE(MsdRadixSortIntegralTests)
{
    TEST(EmptyRange)
    {
        std::vector<int> values;
        msdRadixSort(values);
        CHECK(values.empty());
    }

    TEST(SingleElement)
    {
        std::vector<int> values{-17};
        msdRadixSort(values);
        CHECK_EQUAL(1, static_cast<int>(values.size()));
        CHECK_EQUAL(-17, values.front());
    }

    TEST(Duplicates)
    {
        std::vector<int> values{5, 2, 5, 2, 5, 2, 0, 0, -3, -3};
        msdRadixSort(values);
        CHECK(values == std::vector<int>({-3, -3, 0, 0, 2, 2, 2, 5, 5, 5}));
    }

    TEST(NegativeNumbers)
    {
        std::vector<int> values{-1, -100, 0, 25, -7, 25, -100, 3};
        msdRadixSort(values);
        CHECK(isSorted(values.begin(), values.end()));
        CHECK(values == std::vector<int>({-100, -100, -7, -1, 0, 3, 25, 25}));
    }

    TEST(ExtremeValues)
    {
        std::vector<int> values{INT_MAX, INT_MIN, 0, -1, 1, INT_MIN, INT_MAX};
        msdRadixSort(values);
        CHECK(isSorted(values.begin(), values.end()));
        CHECK_EQUAL(INT_MIN, values.front());
        CHECK_EQUAL(INT_MAX, values.back());
    }

    TEST(SmallSignedAndUnsignedTypes)
    {
        std::vector<short> shorts{-5, 300, -32768, 0, 12, -1, 32767};
        msdRadixSort(shorts);
        CHECK(isSorted(shorts.begin(), shorts.end()));
        CHECK_EQUAL(-32768, static_cast<int>(shorts.front()));
        CHECK_EQUAL(32767, static_cast<int>(shorts.back()));

        std::vector<unsigned int> unsignedValues{7u, 0u, 4000000000u, 12u, 0u, 255u};
        msdRadixSort(unsignedValues);
        CHECK(isSorted(unsignedValues.begin(), unsignedValues.end()));
        CHECK_EQUAL(0u, unsignedValues.front());
        CHECK_EQUAL(4000000000u, unsignedValues.back());

        std::vector<char> chars{'z', 'a', '\0', 'A', ' ', '9'};
        msdRadixSort(chars);
        CHECK(isSorted(chars.begin(), chars.end()));
        CHECK_EQUAL('\0', chars.front());
    }

    TEST(RawArrayOverload)
    {
        int raw[] = {170, 45, 75, 90, 802, 24, 2, 66, -5};
        msdRadixSort(raw);
        CHECK(isSorted(std::begin(raw), std::end(raw)));
        CHECK_EQUAL(-5, raw[0]);
        CHECK_EQUAL(802, raw[8]);
    }
}

SUITE(MsdRadixSortStringTests)
{
    TEST(EmptyAndSingle)
    {
        std::vector<std::string> empty;
        msdRadixSort(empty);
        CHECK(empty.empty());

        std::vector<std::string> single{"only"};
        msdRadixSort(single);
        CHECK(single == std::vector<std::string>({"only"}));
    }

    TEST(CommonPrefixes)
    {
        std::vector<std::string> words{
            "apple", "app", "application", "applesauce", "apply", "ape", "ap"};
        std::vector<std::string> expected = words;
        std::sort(expected.begin(), expected.end());

        msdRadixSort(words);
        CHECK(words == expected);
    }

    TEST(EmptyStringsAndDifferentLengths)
    {
        std::vector<std::string> words{"", "a", "", "ab", "abc", "b", "", "ba"};
        std::vector<std::string> expected = words;
        std::sort(expected.begin(), expected.end());

        msdRadixSort(words);
        CHECK(words == expected);
    }

    TEST(SpecialCharacters)
    {
        std::vector<std::string> words{
            "b", "a b", "A", "a\tb", "~", "0", "a1", "a!", "\x7f", "az"};
        std::vector<std::string> expected = words;
        std::sort(expected.begin(), expected.end());

        msdRadixSort(words);
        CHECK(words == expected);
    }
}

SUITE(MsdRadixSortByKeyTests)
{
    TEST(SortByGrade)
    {
        std::vector<Schoolboy> schoolboys{
            {"Alice", 5, 4.7}, {"Bob", 3, 3.4},  {"Carol", 7, 4.9},
            {"Dave", 2, 4.2},  {"Eve", 5, 4.6}};

        msdRadixSortByKey(
            schoolboys, [](const Schoolboy& s) { return s.grade(); });

        for (std::size_t index = 1; index < schoolboys.size(); ++index) {
            CHECK(schoolboys[index - 1].grade() <= schoolboys[index].grade());
        }
        CHECK(schoolboys.front().name() == "Dave");
        CHECK(schoolboys.back().name() == "Carol");
    }

    TEST(NegativeKeys)
    {
        std::vector<int> values{-10, 5, -3, 0, 7, -20, -10, 1};
        msdRadixSortByKey(values, [](int value) { return value; });
        CHECK(values == std::vector<int>({-20, -10, -10, -3, 0, 1, 5, 7}));
    }

    TEST(StableForEqualKeys)
    {
        std::vector<std::pair<int, int>> values{
            {2, 0}, {1, 1}, {2, 2}, {0, 3}, {1, 4}, {2, 5}, {0, 6}};

        msdRadixSortByKey(values, [](const std::pair<int, int>& value) {
            return value.first;
        });

        std::vector<std::pair<int, int>> expected{
            {0, 3}, {0, 6}, {1, 1}, {1, 4}, {2, 0}, {2, 2}, {2, 5}};
        CHECK(values == expected);
    }
}

SUITE(IsSortedTests)
{
    TEST(EmptyAndSortedRanges)
    {
        std::vector<int> empty;
        CHECK(isSorted(empty.begin(), empty.end()));

        std::vector<int> single{7};
        CHECK(isSorted(single.begin(), single.end()));

        std::vector<int> sorted{1, 1, 2, 3, 3, 9};
        CHECK(isSorted(sorted.begin(), sorted.end()));

        std::vector<int> descending{9, 5, 5, 2, 0, -4};
        CHECK(isSorted(descending.begin(), descending.end(), std::greater<int>{}));
    }

    TEST(DetectsUnsorted)
    {
        std::vector<int> values{1, 3, 2, 4};
        CHECK(!isSorted(values.begin(), values.end()));

        std::vector<int> descending{9, 5, 7, 1};
        CHECK(!isSorted(descending.begin(), descending.end(), std::greater<int>{}));
    }
}

SUITE(SchoolboyTests)
{
    TEST(ValidationThrows)
    {
        CHECK_THROW(Schoolboy("", 5, 4.0), std::invalid_argument);
        CHECK_THROW(Schoolboy("Ann", 0, 4.0), std::invalid_argument);
        CHECK_THROW(Schoolboy("Ann", 12, 4.0), std::invalid_argument);
        CHECK_THROW(Schoolboy("Ann", 5, -0.1), std::invalid_argument);
        CHECK_THROW(Schoolboy("Ann", 5, 5.1), std::invalid_argument);

        bool boundariesAccepted = true;
        try {
            Schoolboy low("Ann", 1, 0.0);
            Schoolboy high("Ann", 11, 5.0);
            (void)low;
            (void)high;
        } catch (...) {
            boundariesAccepted = false;
        }
        CHECK(boundariesAccepted);
    }

    TEST(AccessorsEqualityAndOutput)
    {
        Schoolboy ann("Ann", 5, 4.5);
        CHECK(ann.name() == "Ann");
        CHECK_EQUAL(5, ann.grade());
        CHECK_CLOSE(4.5, ann.averageScore(), 1e-9);

        Schoolboy sameAnn("Ann", 5, 4.5);
        CHECK(ann == sameAnn);

        Schoolboy bob("Bob", 5, 4.5);
        CHECK(ann < bob);
        CHECK(!(bob < ann));

        std::ostringstream stream;
        stream << ann;
        CHECK(stream.str().find("Ann") != std::string::npos);
        CHECK(ann.describe().find("grade 5") != std::string::npos);
        CHECK(ann.describe().find("avg") != std::string::npos);
    }
}
