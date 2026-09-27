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
    std::string left;     
    std::string right;     
    bool isFinal;          

public:
    Rule();
    Rule(const std::string& left, const std::string& right, bool isFinal = false);

    std::string getLeft() const;
    std::string getRight() const;
    bool isFinalRule() const;
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
    std::vector<Rule> rules;

public:
    RuleSet();
    explicit RuleSet(const std::vector<Rule>& rules);

    void addRule(const Rule& rule);
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
    std::string word;      
    RuleSet ruleSet;     
    bool halted;           
    size_t stepCount;     

public:
    MarkovAlgorithm();
    MarkovAlgorithm(const std::string& initialWord, const RuleSet& rules);

    bool step();   
    void run(); 

    std::string getWord() const;
    bool isHalted() const;
    size_t getStepCount() const;

    MarkovAlgorithm& operator++();  
    MarkovAlgorithm operator++(int); 

    bool operator==(const MarkovAlgorithm& other) const; 
    bool operator!=(const MarkovAlgorithm& other) const; 

    friend std::ostream& operator<<(std::ostream& os, const MarkovAlgorithm& ma);
    friend std::istream& operator>>(std::istream& is, MarkovAlgorithm& ma);
};