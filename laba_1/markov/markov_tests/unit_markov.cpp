/**
 * @file unit_markov.cpp
 * @brief Unit-тесты для C++ эмулятора Нормального Алгоритма Маркова (UnitTest++).
 */

#include <UnitTest++/UnitTest++.h>
#include <sstream>
#include <stdexcept>
#include "markov.hpp"

TEST(RuleDefaultCtor) {
    Rule r;
    CHECK_EQUAL("", r.getLeft());
    CHECK_EQUAL("", r.getRight());
    CHECK(!r.isFinalRule());
}

TEST(RuleParameterizedCtor) {
    Rule r("ab", "c", true);
    CHECK_EQUAL("ab", r.getLeft());
    CHECK_EQUAL("c", r.getRight());
    CHECK(r.isFinalRule());
}

TEST(RuleToString) {
    CHECK_EQUAL("a -> b", Rule("a", "b").toString());
}

TEST(RuleToStringFinal) {
    CHECK_EQUAL("a ->. b", Rule("a", "b", true).toString());
}

TEST(RuleEqualityDifferent) {
    CHECK(Rule("a", "b") != Rule("a", "b", true));
    CHECK(Rule("a", "b") != Rule("c", "b"));
    CHECK(Rule("a", "b") != Rule("a", "c"));
}

TEST(RuleStreamOutput) {
    std::ostringstream os;
    os << Rule("ab", "c", true);
    CHECK_EQUAL("ab ->. c", os.str());
}

TEST(RuleStreamInputValid) {
    std::istringstream is("ab -> c");
    Rule r;
    is >> r;
    CHECK_EQUAL("ab", r.getLeft());
    CHECK_EQUAL("c", r.getRight());
    CHECK(!r.isFinalRule());
}

TEST(RuleStreamInputFinal) {
    std::istringstream is("ab ->. c");
    Rule r;
    is >> r;
    CHECK_EQUAL("ab", r.getLeft());
    CHECK_EQUAL("c", r.getRight());
    CHECK(r.isFinalRule());
}

TEST(RuleStreamInputEmptyLeft) {
    std::istringstream is(" -> x");
    Rule r;
    is >> r;
    CHECK_EQUAL("", r.getLeft());
    CHECK_EQUAL("x", r.getRight());
}

TEST(RuleStreamInputTrimsSpaces) {
    std::istringstream is("  ab  ->  c  ");
    Rule r;
    is >> r;
    CHECK_EQUAL("ab", r.getLeft());
    CHECK_EQUAL("c", r.getRight());
}

TEST(RuleStreamInputWithoutArrowFails) {
    std::istringstream is("abc");
    Rule r("x", "y");
    bool ok = static_cast<bool>(is >> r);
    CHECK(!ok);
    CHECK(r == Rule("x", "y"));
}

TEST(RuleSetEmptyInitial) {
    RuleSet set;
    CHECK_EQUAL(0u, set.size());
}

TEST(RuleSetAddRuleFromString) {
    RuleSet set;
    set.addRule("a -> b");
    CHECK_EQUAL(1u, set.size());
    CHECK(set.getRules()[0] == Rule("a", "b"));
}

TEST(RuleSetAddRuleInvalidStringThrows) {
    RuleSet set;
    CHECK_THROW(set.addRule("abc"), std::invalid_argument);
    CHECK_EQUAL(0u, set.size());
}

TEST(RuleSetPreservesOrder) {
    RuleSet set;
    set.addRule(Rule("a", "1"));
    set.addRule(Rule("b", "2"));
    CHECK(set.getRules()[0] == Rule("a", "1"));
    CHECK(set.getRules()[1] == Rule("b", "2"));
}

TEST(RuleSetClear) {
    RuleSet set;
    set.addRule(Rule("a", "b"));
    set.addRule(Rule("c", "d"));
    set.clear();
    CHECK_EQUAL(0u, set.size());
}

TEST(RuleSetStreamOutput) {
    RuleSet set;
    set.addRule(Rule("a", "b"));
    set.addRule(Rule("c", "d", true));
    std::ostringstream os;
    os << set;
    CHECK_EQUAL("a -> b\nc ->. d\n", os.str());
}

TEST(RuleSetStreamInputSkipsBlankLines) {
    std::istringstream is("a -> b\n\nc ->. d\n");
    RuleSet set;
    bool ok = static_cast<bool>(is >> set);
    CHECK(ok);
    CHECK_EQUAL(2u, set.size());
    CHECK(set.getRules()[1] == Rule("c", "d", true));
}

TEST(RuleSetStreamInputReplacesContent) {
    std::istringstream is("a -> b\n");
    RuleSet set;
    set.addRule(Rule("x", "y"));
    set.addRule(Rule("z", "w"));
    is >> set;
    CHECK_EQUAL(1u, set.size());
    CHECK(set.getRules()[0] == Rule("a", "b"));
}

TEST(RuleSetStreamInputInvalidLineFails) {
    std::istringstream is("a -> b\nbad\n");
    RuleSet set;
    set.addRule(Rule("x", "y"));
    bool ok = static_cast<bool>(is >> set);
    CHECK(!ok);
    CHECK_EQUAL(1u, set.size());
    CHECK(set.getRules()[0] == Rule("x", "y"));
}

TEST(MarkovDefaultCtor) {
    MarkovAlgorithm ma;
    CHECK_EQUAL("", ma.getWord());
    CHECK_EQUAL(0u, ma.getStepCount());
    CHECK(!ma.isHalted());
}

TEST(MarkovStepAppliesRule) {
    RuleSet rules;
    rules.addRule(Rule("a", "b"));
    MarkovAlgorithm ma("aa", rules);

    CHECK(ma.step());
    CHECK_EQUAL("ba", ma.getWord());
    CHECK_EQUAL(1u, ma.getStepCount());
    CHECK(!ma.isHalted());
}

TEST(MarkovStepReplacesLeftmostOccurrence) {
    RuleSet rules;
    rules.addRule(Rule("ab", "x"));
    MarkovAlgorithm ma("abab", rules);

    ma.step();
    CHECK_EQUAL("xab", ma.getWord());
}

TEST(MarkovRuleOrderHasPriority) {
    RuleSet rules;
    rules.addRule(Rule("a", "1"));
    rules.addRule(Rule("b", "2"));
    MarkovAlgorithm ma("ba", rules);

    ma.step();
    CHECK_EQUAL("b1", ma.getWord());
}

TEST(MarkovStepHaltsWithoutMatch) {
    RuleSet rules;
    rules.addRule(Rule("a", "b"));
    MarkovAlgorithm ma("xyz", rules);

    CHECK(!ma.step());
    CHECK(ma.isHalted());
    CHECK_EQUAL("xyz", ma.getWord());
    CHECK_EQUAL(0u, ma.getStepCount());
}

TEST(MarkovFinalRuleHalts) {
    RuleSet rules;
    rules.addRule(Rule("a", "b", true));
    MarkovAlgorithm ma("a", rules);

    CHECK(ma.step());
    CHECK_EQUAL("b", ma.getWord());
    CHECK(ma.isHalted());
    CHECK_EQUAL(1u, ma.getStepCount());
    CHECK(!ma.step());
}

TEST(MarkovEmptyLeftPrepends) {
    RuleSet rules;
    rules.addRule(Rule("", "x", true));
    MarkovAlgorithm ma("abc", rules);

    ma.step();
    CHECK_EQUAL("xabc", ma.getWord());
    CHECK(ma.isHalted());
}

TEST(MarkovEmptyRightDeletes) {
    RuleSet rules;
    rules.addRule(Rule("a", ""));
    MarkovAlgorithm ma("banana", rules);

    ma.step();
    CHECK_EQUAL("bnana", ma.getWord());
}

TEST(MarkovRunSortsWord) {
    RuleSet rules;
    rules.addRule(Rule("ab", "ba"));
    MarkovAlgorithm ma("aabb", rules);
    ma.run();

    CHECK(ma.isHalted());
    CHECK_EQUAL("bbaa", ma.getWord());
    CHECK_EQUAL(4u, ma.getStepCount());
}

TEST(MarkovRunRespectsStepLimit) {
    RuleSet rules;
    rules.addRule(Rule("", "a")); // бесконечно дописывает 'a' в начало
    MarkovAlgorithm ma("", rules);
    ma.run(10);

    CHECK(!ma.isHalted());
    CHECK_EQUAL(10u, ma.getStepCount());
    CHECK_EQUAL(10u, ma.getWord().size());
}

TEST(MarkovSetWordResetsState) {
    RuleSet rules;
    rules.addRule(Rule("a", "b"));
    MarkovAlgorithm ma("aa", rules);
    ma.run();
    CHECK(ma.isHalted());
    CHECK_EQUAL("bb", ma.getWord());

    ma.setWord("aaa");
    CHECK_EQUAL("aaa", ma.getWord());
    CHECK(!ma.isHalted());
    CHECK_EQUAL(0u, ma.getStepCount());

    ma.run();
    CHECK_EQUAL("bbb", ma.getWord());
    CHECK_EQUAL(3u, ma.getStepCount());
}

TEST(MarkovOutputStream) {
    RuleSet rules;
    MarkovAlgorithm ma("abc", rules);
    std::ostringstream os;
    os << ma;

    CHECK_EQUAL("Word: abc | Steps: 0 | Halted: No", os.str());
}

int main() {
    return UnitTest::RunAllTests();
}