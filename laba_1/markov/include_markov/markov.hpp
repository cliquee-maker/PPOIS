#pragma once

#include <string>
#include <vector>
#include <iostream>

/**
 * @class Rule
 * @brief Одно правило подстановки для Нормального Алгоритма Маркова.
 */
class Rule {
private:
    std::string left_;     
    std::string right_;     
    bool isFinal_;          

public:
    Rule();
    Rule(const std::string& leftPattern, const std::string& rightPattern, bool isTerminal = false);

    std::string getLeft() const;
    std::string getRight() const;
    bool isFinalRule() const;

    /**
     * @brief Преобразует правило в строковый вид (например, "a -> b" или "a ->. b").
     * @return Строковое представление правила.
     */
    std::string toString() const;

    bool operator==(const Rule& other) const;     
    bool operator!=(const Rule& other) const;     

    friend std::ostream& operator<<(std::ostream& os, const Rule& rule);
    friend std::istream& operator>>(std::istream& is, Rule& rule);
};


/**
 * @class RuleSet
 * @brief Класс для хранения, управления и поиска в упорядоченном наборе правил.
 */
class RuleSet {
private:
    std::vector<Rule> rules_;

public:
    RuleSet();
    explicit RuleSet(const std::vector<Rule>& rulesList);

    /**
     * @brief Добавляет правило в конец списка.
     * @param rule Объект правила.
     */
    void addRule(const Rule& rule);

    /**
     * @brief Разбирает строку и добавляет правило.
     * @param ruleStr Строка формата "left -> right" или "left ->. right".
     * @throws std::invalid_argument Если формат строки неверный.
     */
    void addRule(const std::string& ruleStr);
    void clear();

    const std::vector<Rule>& getRules() const;
    size_t size() const;

    bool operator==(const RuleSet& other) const;     
    bool operator!=(const RuleSet& other) const;    

    friend std::ostream& operator<<(std::ostream& os, const RuleSet& rs);
    friend std::istream& operator>>(std::istream& is, RuleSet& rs);
};


/**
 * @class MarkovAlgorithm
 * @brief Управляющий класс алгоритма Маркова над входным словом.
 */
class MarkovAlgorithm {
private:
    std::string word_;
    RuleSet ruleSet_;
    bool halted_;
    size_t stepCount_;

public:
    MarkovAlgorithm();
    MarkovAlgorithm(const std::string& initialWord, const RuleSet& initialRules);

    /**
     * @brief Выполняет ровно один шаг подстановки (первого применимого правила).
     * @return true, если шаг успешно выполнен; false, если ни одно правило не применимо или алгоритм остановлен.
     */
    bool step();

    /**
     * @brief Запускает алгоритм до полной остановки или превышения лимита шагов.
     * @param maxSteps Максимально допустимое количество шагов (защита от зацикливания).
     */
    void run(size_t maxSteps = 100000);

    /**
     * @brief Задает новое обрабатываемое слово и сбрасывает счетчики/флаг остановки.
     * @param newWord Новое слово.
     */
    void setWord(const std::string& newWord);
    const std::string& getWord() const;
    bool isHalted() const;
    size_t getStepCount() const;

    friend std::ostream& operator<<(std::ostream& os, const MarkovAlgorithm& ma);
};