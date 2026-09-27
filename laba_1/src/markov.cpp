#include "markov.hpp"
#include <sstream>
#include <algorithm>

Rule::Rule() : isFinal(false) {}

Rule::Rule(const std::string& left, const std::string& right, bool isFinal)
    : left(left), right(right), isFinal(isFinal) {}

std::string Rule::getLeft() const { return left; }
std::string Rule::getRight() const { return right; }
bool Rule::isFinalRule() const { return isFinal; }

std::string Rule::toString() const {
    return left + (isFinal ? " ->. " : " -> ") + right;
}

bool Rule::operator==(const Rule& other) const {
    return left == other.left && right == other.right && isFinal == other.isFinal;
}

bool Rule::operator!=(const Rule& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Rule& rule) {
    os << rule.toString();
    return os;
}

std::istream& operator>>(std::istream& is, Rule& rule) {
    std::string line;
    if (std::getline(is, line)) {
        size_t arrowPos = line.find("->.");
        if (arrowPos != std::string::npos) {
            rule.left = line.substr(0, arrowPos);
            rule.right = line.substr(arrowPos + 3);
            rule.isFinal = true;
        } else {
            arrowPos = line.find("->");
            if (arrowPos != std::string::npos) {
                rule.left = line.substr(0, arrowPos);
                rule.right = line.substr(arrowPos + 2);
                rule.isFinal = false;
            }
        }
        rule.left.erase(rule.left.find_last_not_of(" \t") + 1);
        rule.right.erase(0, rule.right.find_first_not_of(" \t"));
    }
    return is;
}

RuleSet::RuleSet() = default;

RuleSet::RuleSet(const std::vector<Rule>& rules) : rules(rules) {}

void RuleSet::addRule(const Rule& rule) {
    rules.push_back(rule);
}

void RuleSet::addRule(const std::string& ruleStr) {
    std::istringstream iss(ruleStr);
    Rule r;
    if (iss >> r) {
        rules.push_back(r);
    }
}

void RuleSet::clear() {
    rules.clear();
}

const std::vector<Rule>& RuleSet::getRules() const {
    return rules;
}

size_t RuleSet::size() const {
    return rules.size();
}

bool RuleSet::operator==(const RuleSet& other) const {
    return rules == other.rules;
}

bool RuleSet::operator!=(const RuleSet& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const RuleSet& rs) {
    for (const auto& rule : rs.rules) {
        os << rule << "\n";
    }
    return os;
}

std::istream& operator>>(std::istream& is, RuleSet& rs) {
    Rule rule;
    while (is >> rule) {
        rs.addRule(rule);
    }
    return is;
}

MarkovAlgorithm::MarkovAlgorithm() : halted(false), stepCount(0) {}

MarkovAlgorithm::MarkovAlgorithm(const std::string& initialWord, const RuleSet& rules)
    : word(initialWord), ruleSet(rules), halted(false), stepCount(0) {}

bool MarkovAlgorithm::step() {
    if (halted) return false;

    for (const auto& rule : ruleSet.getRules()) {
        size_t pos = word.find(rule.getLeft());
        if (pos != std::string::npos) {
            word.replace(pos, rule.getLeft().length(), rule.getRight());
            stepCount++;
            if (rule.isFinalRule()) {
                halted = true;
            }
            return true;
        }
    }
    halted = true;
    return false;
}

void MarkovAlgorithm::run() {
    while (!halted) {
        step();
    }
}

std::string MarkovAlgorithm::getWord() const { return word; }
bool MarkovAlgorithm::isHalted() const { return halted; }
size_t MarkovAlgorithm::getStepCount() const { return stepCount; }

MarkovAlgorithm& MarkovAlgorithm::operator++() {
    step();
    return *this;
}

MarkovAlgorithm MarkovAlgorithm::operator++(int) {
    MarkovAlgorithm temp = *this;
    step();
    return temp;
}

bool MarkovAlgorithm::operator==(const MarkovAlgorithm& other) const {
    return word == other.word && ruleSet == other.ruleSet &&
           halted == other.halted && stepCount == other.stepCount;
}

bool MarkovAlgorithm::operator!=(const MarkovAlgorithm& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const MarkovAlgorithm& ma) {
    os << "Word: " << ma.word << " | Steps: " << ma.stepCount 
       << " | Halted: " << (ma.halted ? "Yes" : "No");
    return os;
}

std::istream& operator>>(std::istream& is, MarkovAlgorithm& ma) {
    is >> ma.word;
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    is >> ma.ruleSet;
    ma.halted = false;
    ma.stepCount = 0;
    return is;
}