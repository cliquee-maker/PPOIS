#include "markov.hpp"

#include <sstream>
#include <stdexcept>

namespace {

const char* const kWhitespace = " \t\r\n";

bool isBlank(const std::string& s) {
    return s.find_first_not_of(kWhitespace) == std::string::npos;
}

std::string trim(const std::string& s) {
    size_t begin = s.find_first_not_of(kWhitespace);
    if (begin == std::string::npos) return "";
    size_t end = s.find_last_not_of(kWhitespace);
    return s.substr(begin, end - begin + 1);
}
} 

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
    return os << rule.toString();
}

std::istream& operator>>(std::istream& is, Rule& rule) {
    std::string line;
    if (!std::getline(is, line)) return is;

    size_t arrowPos = line.find("->");
    if (arrowPos == std::string::npos) {
        is.setstate(std::ios::failbit);
        return is;
    }

    bool fin = arrowPos + 2 < line.size() && line[arrowPos + 2] == '.';
    size_t rightStart = arrowPos + (fin ? 3 : 2);

    rule = Rule(trim(line.substr(0, arrowPos)), trim(line.substr(rightStart)), fin);
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
    if (!(iss >> r)) {
        throw std::invalid_argument("Invalid rule format: " + ruleStr);
    }
    rules.push_back(r);
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
        os << rule << '\n';
    }
    return os;
}

std::istream& operator>>(std::istream& is, RuleSet& rs) {
    RuleSet parsed;
    std::string line;
    while (std::getline(is, line)) {
        if (isBlank(line)) continue;
        std::istringstream iss(line);
        Rule rule;
        if (!(iss >> rule)) {
            is.setstate(std::ios::failbit);
            return is;
        }
        parsed.addRule(rule);
    }
    is.clear(is.rdstate() & ~std::ios::failbit);
    rs = parsed;
    return is;
}


MarkovAlgorithm::MarkovAlgorithm() : halted(false), stepCount(0) {}

MarkovAlgorithm::MarkovAlgorithm(const std::string& initialWord, const RuleSet& rules)
    : word(initialWord), ruleSet(rules), halted(false), stepCount(0) {}

bool MarkovAlgorithm::step() {
    if (halted) return false;

    for (const auto& rule : ruleSet.getRules()) {
        const std::string& left = rule.getLeft();
        size_t pos = word.find(left);
        if (pos != std::string::npos) {
            word.replace(pos, left.length(), rule.getRight());
            ++stepCount;
            if (rule.isFinalRule()) {
                halted = true;
            }
            return true;
        }
    }
    halted = true;
    return false;
}

void MarkovAlgorithm::run(size_t maxSteps) {
    for (size_t i = 0; i < maxSteps && !halted; ++i) {
        step();
    }
}

void MarkovAlgorithm::setWord(const std::string& newWord) {
    word = newWord;
    halted = false;
    stepCount = 0;
}

const std::string& MarkovAlgorithm::getWord() const { return word; }
bool MarkovAlgorithm::isHalted() const { return halted; }
size_t MarkovAlgorithm::getStepCount() const { return stepCount; }

std::ostream& operator<<(std::ostream& os, const MarkovAlgorithm& ma) {
    os << "Word: " << ma.word << " | Steps: " << ma.stepCount
       << " | Halted: " << (ma.halted ? "Yes" : "No");
    return os;
}