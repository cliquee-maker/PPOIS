#pragma once

#include <deque>
#include <iostream>
#include <string>
#include <vector>

/**
 * @enum Direction
 * @brief Направление движения головки машины Тьюринга.
 */
enum class Direction { Left, Right, Stay };

char directionToChar(Direction dir);

/// @throws std::invalid_argument, если символ не 'L', 'R' или 'S'.
Direction charToDirection(char c);

/**
 * @class TMRule
 * @brief Одно правило перехода: (state, readSymbol) -> (newState, writeSymbol, direction).
 *        Текстовый формат: "q0 a q1 b R".
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

    /**
     * @brief Проверяет применимость правила к данному состоянию и символу.
     * @param curState Проверяемое состояние.
     * @param symbol Проверяемый символ.
     * @return true, если правило подходит, иначе false.
     */
    bool matches(const std::string& curState, char symbol) const;
    std::string toString() const;

    bool operator==(const TMRule& other) const;
    bool operator!=(const TMRule& other) const;

    friend std::ostream& operator<<(std::ostream& os, const TMRule& rule);
    friend std::istream& operator>>(std::istream& is, TMRule& rule);
};


/**
 * @class TMRuleSet
 * @brief Программа машины Тьюринга — набор правил переходов.
 *        Формат ввода/вывода: одно правило на строку.
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
    /**
     * @brief Поиск применимого правила.
     * @param state Текущее состояние.
     * @param symbol Считываемый символ.
     * @return Указатель на найденное правило или nullptr, если правило не найдено.
     */
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
 * @brief Машина Тьюринга: лента, программа и текущее состояние.
 *        Останавливается, когда нет применимого правила для (состояния, символа).
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
    /**
     * @brief Выполняет один шаг машины.
     * @return true, если шаг выполнен; false, если машина остановилась.
     */
    bool step();
    /** @brief Выполняет программу до момента остановки. */
    void run();

    void setTape(const std::string& content);
    std::string getTapeString() const;
    long getHeadPosition() const;
    std::string getState() const;
    bool isHalted() const;
    size_t getStepCount() const;

    friend std::ostream& operator<<(std::ostream& os, const TuringMachine& tm);
};