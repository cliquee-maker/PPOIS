#pragma once

#include <string>
#include <vector>
#include <deque>
#include <iostream>

enum class Direction { Left, Right, Stay };

char directionToChar(Direction dir);
Direction charToDirection(char c);


/**
 * @class TMRule
 * @brief Одно правило перехода машины Тьюринга: (state, readSymbol) -> (newState, writeSymbol, direction).
 */
class TMRule {
private:
    std::string state;
    char readSymbol;
    std::string newState;
    char writeSymbol;
    Direction direction;

public:
    TMRule();
    TMRule(const std::string& state, char readSymbol,
           const std::string& newState, char writeSymbol, Direction direction);

    std::string getState() const;
    char getReadSymbol() const;
    std::string getNewState() const;
    char getWriteSymbol() const;
    Direction getDirection() const;

    bool matches(const std::string& state, char symbol) const;
    std::string toString() const;

    bool operator==(const TMRule& other) const;
    bool operator!=(const TMRule& other) const;

    friend std::ostream& operator<<(std::ostream& os, const TMRule& rule);
    friend std::istream& operator>>(std::istream& is, TMRule& rule);
};


/**
 * @class TMRuleSet
 * @brief Программа машины Тьюринга — набор правил переходов.
 */
class TMRuleSet {
private:
    std::vector<TMRule> rules;

public:
    TMRuleSet();
    explicit TMRuleSet(const std::vector<TMRule>& rules);

    void addRule(const TMRule& rule);
    void addRule(const std::string& ruleStr);
    bool removeRule(const std::string& state, char symbol);
    void clear();

    const TMRule* findRule(const std::string& state, char symbol) const;
    const std::vector<TMRule>& getRules() const;
    size_t size() const;

    bool operator==(const TMRuleSet& other) const;
    bool operator!=(const TMRuleSet& other) const;

    friend std::ostream& operator<<(std::ostream& os, const TMRuleSet& rs);
    friend std::istream& operator>>(std::istream& is, TMRuleSet& rs);
};


/**
 * @class TuringMachine
 * @brief Лента (хранится внутри как deque<char> с кареткой) + программа + текущее состояние.
 *        Останавливается, когда для текущей пары (состояние, символ) нет применимого правила.
 */
class TuringMachine {
private:
    std::deque<char> tape;
    size_t headIndex;
    char blank;
    TMRuleSet ruleSet;
    std::string currentState;
    bool halted;
    size_t stepCount;
    
    char readCell() const;
    void writeCell(char symbol);
    void moveHead(Direction dir);
public:
    TuringMachine();
    TuringMachine(const std::string& initialTape, const TMRuleSet& rules,
                  const std::string& startState, char blank = '_');

    bool step();
    void run();

    void setTape(const std::string& content);
    std::string getTapeString() const;
    long getHeadPosition() const;
    std::string getState() const;
    bool isHalted() const;
    size_t getStepCount() const;

    TuringMachine& operator++();
    TuringMachine operator++(int);

    bool operator==(const TuringMachine& other) const;
    bool operator!=(const TuringMachine& other) const;

    friend std::ostream& operator<<(std::ostream& os, const TuringMachine& tm);
    friend std::istream& operator>>(std::istream& is, TuringMachine& tm);
};