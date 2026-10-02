#include "markov.hpp"

#include <sstream>
#include <stdexcept>

namespace {

const char* const kWhitespace = " \t\r\n";

bool isBlank(const std::string& str) {
    return str.find_first_not_of(kWhitespace) == std::string::npos;
}

std::string trim(const std::string& str) {
    size_t beginPos = str.find_first_not_of(kWhitespace);
    if (beginPos == std::string::npos) return "";
    size_t endPos = str.find_last_not_of(kWhitespace);
    return str.substr(beginPos, endPos - beginPos + 1);
}

} // namespace

Rule::Rule() : isFinal_(false) {}

Rule::Rule(const std::string& leftPattern, const std::string& rightPattern, bool isTerminal)
    : left_(leftPattern), right_(rightPattern), isFinal_(isTerminal) {}

std::string Rule::getLeft() const { return left_; }
std::string Rule::getRight() const { return right_; }
bool Rule::isFinalRule() const { return isFinal_; }

std::string Rule::toString() const {
    return left_ + (isFinal_ ? " ->. " : " -> ") + right_;
}

bool Rule::operator==(const Rule& other) const {
    return left_ == other.left_ && right_ == other.right_ && isFinal_ == other.isFinal_;
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

    bool terminalFlag = arrowPos + 2 < line.size() && line[arrowPos + 2] == '.';
    size_t rightStartPos = arrowPos + (terminalFlag ? 3 : 2);

    rule = Rule(trim(line.substr(0, arrowPos)), trim(line.substr(rightStartPos)), terminalFlag);
    return is;
}

RuleSet::RuleSet() = default;

RuleSet::RuleSet(const std::vector<Rule>& rulesList) : rules_(rulesList) {}

void RuleSet::addRule(const Rule& rule) {
    rules_.push_back(rule);
}

void RuleSet::addRule(const std::string& ruleStr) {
    std::istringstream iss(ruleStr);
    Rule parsedRule;
    if (!(iss >> parsedRule)) {
        throw std::invalid_argument("Invalid rule format: " + ruleStr);
    }
    rules_.push_back(parsedRule);
}

void RuleSet::clear() {
    rules_.clear();
}

const std::vector<Rule>& RuleSet::getRules() const {
    return rules_;
}

size_t RuleSet::size() const {
    return rules_.size();
}

bool RuleSet::operator==(const RuleSet& other) const {
    return rules_ == other.rules_;
}

bool RuleSet::operator!=(const RuleSet& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const RuleSet& rs) {
    for (const auto& rule : rs.rules_) {
        os << rule << '\n';
    }
    return os;
}

std::istream& operator>>(std::istream& is, RuleSet& rs) {
    RuleSet parsedSet;
    std::string line;
    while (std::getline(is, line)) {
        if (isBlank(line)) continue;
        std::istringstream iss(line);
        Rule currentRule;
        if (!(iss >> currentRule)) {
            is.setstate(std::ios::failbit);
            return is;
        }
        parsedSet.addRule(currentRule);
    }
    is.clear(is.rdstate() & ~std::ios::failbit);
    rs = parsedSet;
    return is;
}


MarkovAlgorithm::MarkovAlgorithm() : halted_(false), stepCount_(0) {}

MarkovAlgorithm::MarkovAlgorithm(const std::string& initialWord, const RuleSet& initialRules)
    : word_(initialWord), ruleSet_(initialRules), halted_(false), stepCount_(0) {}

bool MarkovAlgorithm::step() {
    if (halted_) return false;

    for (const auto& rule : ruleSet_.getRules()) {
        const std::string& leftPattern = rule.getLeft();
        size_t matchPos = word_.find(leftPattern);
        if (matchPos != std::string::npos) {
            word_.replace(matchPos, leftPattern.length(), rule.getRight());
            ++stepCount_;
            if (rule.isFinalRule()) {
                halted_ = true;
            }
            return true;
        }
    }
    halted_ = true;
    return false;
}

void MarkovAlgorithm::run(size_t maxSteps) {
    for (size_t stepIdx = 0; stepIdx < maxSteps && !halted_; ++stepIdx) {
        step();
    }
}

void MarkovAlgorithm::setWord(const std::string& newWord) {
    word_ = newWord;
    halted_ = false;
    stepCount_ = 0;
}

const std::string& MarkovAlgorithm::getWord() const { return word_; }
bool MarkovAlgorithm::isHalted() const { return halted_; }
size_t MarkovAlgorithm::getStepCount() const { return stepCount_; }

std::ostream& operator<<(std::ostream& os, const MarkovAlgorithm& ma) {
    os << "Word: " << ma.word_ << " | Steps: " << ma.stepCount_
       << " | Halted: " << (ma.halted_ ? "Yes" : "No");
    return os;
}