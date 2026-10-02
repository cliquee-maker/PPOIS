#include "turing.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>

char directionToChar(Direction dir) {
    switch (dir) {
        case Direction::Left: return 'L';
        case Direction::Right: return 'R';
        case Direction::Stay: return 'S';
        default: return 'S';
    }
}

Direction charToDirection(char c) {
    switch (c) {
        case 'L': case 'l': return Direction::Left;
        case 'R': case 'r': return Direction::Right;
        case 'S': case 's': return Direction::Stay;
        default: throw std::invalid_argument("Неверный символ направления: должен быть L, R или S");
    }
}

TMRule::TMRule() : readSymbol_('_'), writeSymbol_('_'), direction_(Direction::Stay) {}

TMRule::TMRule(const std::string& state, char readSymbol,
               const std::string& newState, char writeSymbol, Direction direction)
    : state_(state), readSymbol_(readSymbol), newState_(newState),
      writeSymbol_(writeSymbol), direction_(direction) {}

std::string TMRule::getState() const { return state_; }
char TMRule::getReadSymbol() const { return readSymbol_; }
std::string TMRule::getNewState() const { return newState_; }
char TMRule::getWriteSymbol() const { return writeSymbol_; }
Direction TMRule::getDirection() const { return direction_; }

bool TMRule::matches(const std::string& st, char symbol) const {
    return state_ == st && readSymbol_ == symbol;
}

std::string TMRule::toString() const {
    std::ostringstream oss;
    oss << state_ << " " << readSymbol_ << " -> " 
        << newState_ << " " << writeSymbol_ << " " << directionToChar(direction_);
    return oss.str();
}

bool TMRule::operator==(const TMRule& other) const {
    return state_ == other.state_ && readSymbol_ == other.readSymbol_ &&
           newState_ == other.newState_ && writeSymbol_ == other.writeSymbol_ &&
           direction_ == other.direction_;
}

bool TMRule::operator!=(const TMRule& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const TMRule& rule) {
    os << rule.toString();
    return os;
}

std::istream& operator>>(std::istream& is, TMRule& rule) {
    std::string arrow;
    char dirChar;
    if (is >> rule.state_ >> rule.readSymbol_ >> arrow >> rule.newState_ >> rule.writeSymbol_ >> dirChar) {
        rule.direction_ = charToDirection(dirChar);
    }
    return is;
}

TMRuleSet::TMRuleSet() = default;

TMRuleSet::TMRuleSet(const std::vector<TMRule>& rules) : rules_(rules) {}

void TMRuleSet::addRule(const TMRule& rule) {
    rules_.push_back(rule);
}

void TMRuleSet::addRule(const std::string& ruleStr) {
    std::istringstream iss(ruleStr);
    TMRule r;
    if (iss >> r) {
        rules_.push_back(r);
    }
}

bool TMRuleSet::removeRule(const std::string& state, char symbol) {
    auto it = std::remove_if(rules_.begin(), rules_.end(), [&](const TMRule& r) { return r.matches(state, symbol); });
    if (it != rules_.end()) {
        rules_.erase(it, rules_.end());
        return true;
    }
    return false;
}

void TMRuleSet::clear() {
    rules_.clear();
}

const TMRule* TMRuleSet::findRule(const std::string& state, char symbol) const {
    for (const auto& rule : rules_) {
        if (rule.matches(state, symbol)) {
            return &rule;
        }
    }
    return nullptr;
}

const std::vector<TMRule>& TMRuleSet::getRules() const {
    return rules_;
}

size_t TMRuleSet::size() const {
    return rules_.size();
}

bool TMRuleSet::operator==(const TMRuleSet& other) const {
    return rules_ == other.rules_;
}

bool TMRuleSet::operator!=(const TMRuleSet& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const TMRuleSet& rs) {
    for (const auto& rule : rs.rules_) {
        os << rule << "\n";
    }
    return os;
}

std::istream& operator>>(std::istream& is, TMRuleSet& rs) {
    TMRule rule;
    while (is >> rule) {
        rs.addRule(rule);
    }
    return is;
}

TuringMachine::TuringMachine() : headIndex_(0), blank_('_'), halted_(true), stepCount_(0) {}

TuringMachine::TuringMachine(const std::string& initialTape, const TMRuleSet& rules,
                             const std::string& startState, char blank)
    : headIndex_(0), blank_(blank), ruleSet_(rules), currentState_(startState),
      halted_(false), stepCount_(0) {
    setTape(initialTape);
}

char TuringMachine::readCell() const {
    if (headIndex_ >= tape_.size()) return blank_;
    return tape_[headIndex_];
}

void TuringMachine::writeCell(char symbol) {
    if (headIndex_ >= tape_.size()) {
        tape_.resize(headIndex_ + 1, blank_);
    }
    tape_[headIndex_] = symbol;
}

void TuringMachine::moveHead(Direction dir) {
    if (dir == Direction::Left) {
        if (headIndex_ == 0) {
            tape_.push_front(blank_);
        } else {
            headIndex_--;
        }
    } else if (dir == Direction::Right) {
        headIndex_++;
        if (headIndex_ >= tape_.size()) {
            tape_.push_back(blank_);
        }
    }
}

bool TuringMachine::step() {
    if (halted_) return false;

    char currentSymbol = readCell();
    const TMRule* rule = ruleSet_.findRule(currentState_, currentSymbol);

    if (!rule) {
        halted_ = true;
        return false;
    }

    writeCell(rule->getWriteSymbol());
    currentState_ = rule->getNewState();
    moveHead(rule->getDirection());
    stepCount_++;

    return true;
}

void TuringMachine::run() {
    while (!halted_) {
        step();
    }
}

void TuringMachine::setTape(const std::string& content) {
    tape_.clear();
    for (char c : content) {
        tape_.push_back(c);
    }
    if (tape_.empty()) {
        tape_.push_back(blank_);
    }
    headIndex_ = 0;
    stepCount_ = 0;
    halted_ = false;
}

std::string TuringMachine::getTapeString() const {
    return std::string(tape_.begin(), tape_.end());
}

long TuringMachine::getHeadPosition() const {
    return static_cast<long>(headIndex_);
}

std::string TuringMachine::getState() const { return currentState_; }
bool TuringMachine::isHalted() const { return halted_; }
size_t TuringMachine::getStepCount() const { return stepCount_; }

std::ostream& operator<<(std::ostream& os, const TuringMachine& tm) {
    os << "State: " << tm.currentState_ << " | Steps: " << tm.stepCount_ << "\nTape: ";
    for (size_t i = 0; i < tm.tape_.size(); ++i) {
        if (i == tm.headIndex_) os << "[" << tm.tape_[i] << "]";
        else os << tm.tape_[i];
    }
    return os;
}