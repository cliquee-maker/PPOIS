/**
 * @file tests.cpp
 * @brief Unit-тесты для C++ эмулятора Машины Тьюринга (UnitTest++).
 */

#include <UnitTest++/UnitTest++.h>
#include <sstream>
#include <stdexcept>
#include "turing_machine.hpp"

TEST(DirectionToCharValid) {
    CHECK_EQUAL('L', directionToChar(Direction::Left));
    CHECK_EQUAL('R', directionToChar(Direction::Right));
    CHECK_EQUAL('S', directionToChar(Direction::Stay));
}

TEST(CharToDirectionValidUpper) {
    CHECK(Direction::Left == charToDirection('L'));
    CHECK(Direction::Right == charToDirection('R'));
    CHECK(Direction::Stay == charToDirection('S'));
}

TEST(CharToDirectionValidLower) {
    CHECK(Direction::Left == charToDirection('l'));
    CHECK(Direction::Right == charToDirection('r'));
    CHECK(Direction::Stay == charToDirection('s'));
}

TEST(CharToDirectionInvalidThrows) {
    CHECK_THROW(charToDirection('X'), std::invalid_argument);
    CHECK_THROW(charToDirection('1'), std::invalid_argument);
}

TEST(TMRuleDefaultCtor) {
    TMRule r;
    CHECK_EQUAL('_', r.getReadSymbol());
    CHECK_EQUAL('_', r.getWriteSymbol());
    CHECK(Direction::Stay == r.getDirection());
}

TEST(TMRuleParameterizedCtor) {
    TMRule r("q0", '1', "q1", '0', Direction::Right);
    CHECK_EQUAL("q0", r.getState());
    CHECK_EQUAL('1', r.getReadSymbol());
    CHECK_EQUAL("q1", r.getNewState());
    CHECK_EQUAL('0', r.getWriteSymbol());
    CHECK(Direction::Right == r.getDirection());
}

TEST(TMRuleMatchesTrue) {
    TMRule r("q0", 'a', "q1", 'b', Direction::Left);
    CHECK(r.matches("q0", 'a'));
}

TEST(TMRuleMatchesFalseState) {
    TMRule r("q0", 'a', "q1", 'b', Direction::Left);
    CHECK(!r.matches("q1", 'a'));
}

TEST(TMRuleMatchesFalseSymbol) {
    TMRule r("q0", 'a', "q1", 'b', Direction::Left);
    CHECK(!r.matches("q0", 'b'));
}

TEST(TMRuleToString) {
    TMRule r("q0", '1', "q1", '0', Direction::Right);
    CHECK_EQUAL("q0 1 -> q1 0 R", r.toString());
}

TEST(TMRuleEqualitySame) {
    TMRule r1("q0", '1', "q1", '0', Direction::Right);
    TMRule r2("q0", '1', "q1", '0', Direction::Right);
    CHECK(r1 == r2);
}

TEST(TMRuleEqualityDifferent) {
    TMRule r1("q0", '1', "q1", '0', Direction::Right);
    TMRule r2("q0", '1', "q1", '0', Direction::Left);
    CHECK(r1 != r2);
}

TEST(TMRuleStreamOutput) {
    TMRule r("q1", '0', "q2", '1', Direction::Stay);
    std::ostringstream os;
    os << r;
    CHECK_EQUAL("q1 0 -> q2 1 S", os.str());
}

TEST(TMRuleStreamInputValid) {
    std::istringstream is("q0 1 -> q1 0 L");
    TMRule r;
    is >> r;
    CHECK_EQUAL("q0", r.getState());
    CHECK_EQUAL('1', r.getReadSymbol());
    CHECK_EQUAL("q1", r.getNewState());
    CHECK_EQUAL('0', r.getWriteSymbol());
    CHECK(Direction::Left == r.getDirection());
}

TEST(TMRuleSetEmptyInitial) {
    TMRuleSet set;
    CHECK_EQUAL(0u, set.size());
}

TEST(TMRuleSetAddRule) {
    TMRuleSet set;
    set.addRule(TMRule("q0", '0', "q0", '1', Direction::Right));
    CHECK_EQUAL(1u, set.size());
}

TEST(TMRuleSetFindRuleFound) {
    TMRuleSet set;
    TMRule r("q0", '1', "q1", '0', Direction::Right);
    set.addRule(r);
    
    const TMRule* found = set.findRule("q0", '1');
    CHECK(found != nullptr);
    CHECK(*found == r);
}

TEST(TMRuleSetFindRuleNotFound) {
    TMRuleSet set;
    set.addRule(TMRule("q0", '1', "q1", '0', Direction::Right));
    
    CHECK(set.findRule("q0", '0') == nullptr);
    CHECK(set.findRule("q1", '1') == nullptr);
}

TEST(TMRuleSetRemoveRuleSuccess) {
    TMRuleSet set;
    set.addRule(TMRule("q0", '1', "q1", '0', Direction::Right));
    
    CHECK(set.removeRule("q0", '1'));
    CHECK_EQUAL(0u, set.size());
}

TEST(TMRuleSetRemoveRuleNonExistent) {
    TMRuleSet set;
    set.addRule(TMRule("q0", '1', "q1", '0', Direction::Right));
    
    CHECK(!set.removeRule("q0", '0'));
    CHECK_EQUAL(1u, set.size());
}

TEST(TMRuleSetClear) {
    TMRuleSet set;
    set.addRule(TMRule("q0", '0', "q0", '1', Direction::Right));
    set.addRule(TMRule("q0", '1', "q1", '0', Direction::Left));
    set.clear();
    CHECK_EQUAL(0u, set.size());
}

TEST(TMRuleSetEquality) {
    TMRuleSet s1, s2;
    TMRule r("q0", '1', "q1", '0', Direction::Right);
    s1.addRule(r);
    s2.addRule(r);
    CHECK(s1 == s2);
}

TEST(TMDefaultCtor) {
    TuringMachine tm;
    CHECK_EQUAL(0l, tm.getHeadPosition());
    CHECK_EQUAL(0u, tm.getStepCount());
    CHECK(tm.isHalted());
}

TEST(TMParameterizedCtorNormal) {
    TMRuleSet rules;
    TuringMachine tm("1101", rules, "q0");
    CHECK_EQUAL("1101", tm.getTapeString());
    CHECK_EQUAL(0l, tm.getHeadPosition());
    CHECK_EQUAL(0u, tm.getStepCount());
    CHECK_EQUAL("q0", tm.getState());
    CHECK(!tm.isHalted());
}

TEST(TMSetTapeEmptyFallbackToBlank) {
    TuringMachine tm;
    tm.setTape("");
    CHECK_EQUAL("_", tm.getTapeString());
}

TEST(TMStepExecutesRule) {
    TMRuleSet rules;
    rules.addRule(TMRule("q0", '1', "q1", '0', Direction::Right));
    TuringMachine tm("1", rules, "q0");
    
    CHECK(tm.step());
    CHECK_EQUAL("0_", tm.getTapeString());
    CHECK_EQUAL("q1", tm.getState());
    CHECK_EQUAL(1l, tm.getHeadPosition());
    CHECK_EQUAL(1u, tm.getStepCount());
}

TEST(TMStepHaltsWithoutRule) {
    TMRuleSet rules; 
    TuringMachine tm("1", rules, "q0");
    
    CHECK(!tm.step());
    CHECK(tm.isHalted());
}

TEST(TMTapeExpandRight) {
    TMRuleSet rules;
    rules.addRule(TMRule("q0", 'a', "q0", 'b', Direction::Right));
    TuringMachine tm("a", rules, "q0");
    
    tm.step();
    CHECK_EQUAL("b_", tm.getTapeString());
    CHECK_EQUAL(1l, tm.getHeadPosition());
}

TEST(TMTapeExpandLeft) {
    TMRuleSet rules;
    rules.addRule(TMRule("q0", 'a', "q0", 'b', Direction::Left));
    TuringMachine tm("a", rules, "q0");
    
    tm.step();
    CHECK_EQUAL("_b", tm.getTapeString());
    CHECK_EQUAL(0l, tm.getHeadPosition());
}

TEST(TMRunBinaryIncrement) {
    TMRuleSet rules;
    rules.addRule(TMRule("q0", '1', "q0", '1', Direction::Right));
    rules.addRule(TMRule("q0", '0', "q0", '0', Direction::Right));
    rules.addRule(TMRule("q0", '_', "q1", '_', Direction::Left));
    rules.addRule(TMRule("q1", '1', "q1", '0', Direction::Left));
    rules.addRule(TMRule("q1", '0', "q2", '1', Direction::Stay));
    rules.addRule(TMRule("q1", '_', "q2", '1', Direction::Stay));

    TuringMachine tm("11", rules, "q0");
    tm.run();

    CHECK(tm.isHalted());
    CHECK_EQUAL("100_", tm.getTapeString());
}

TEST(TMStepCountIncrements) {
    TMRuleSet rules;
    rules.addRule(TMRule("q0", '1', "q0", '1', Direction::Right));
    TuringMachine tm("111", rules, "q0");
    
    tm.step();
    tm.step();
    CHECK_EQUAL(2u, tm.getStepCount());
}

TEST(TMOutputStream) {
    TMRuleSet rules;
    TuringMachine tm("10", rules, "q0");
    std::ostringstream os;
    os << tm;
    
    CHECK_EQUAL("State: q0 | Steps: 0\nTape: [1]0", os.str());
}

int main() {
    return UnitTest::RunAllTests();
}