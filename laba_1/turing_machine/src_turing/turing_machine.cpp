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

TMRule::TMRule() : readSymbol('_'), writeSymbol('_'), direction(Direction::Stay) {}

TMRule::TMRule(const std::string& state, char readSymbol,
               const std::string& newState, char writeSymbol, Direction direction)
    : state(state), readSymbol(readSymbol), newState(newState),
      writeSymbol(writeSymbol), direction(direction) {}

std::string TMRule::getState() const { return state; }
char TMRule::getReadSymbol() const { return readSymbol; }
std::string TMRule::getNewState() const { return newState; }
char TMRule::getWriteSymbol() const { return writeSymbol; }
Direction TMRule::getDirection() const { return direction; }

bool TMRule::matches(const std::string& st, char symbol) const {
    return state == st && readSymbol == symbol;
}

std::string TMRule::toString() const {
    std::ostringstream oss;
    oss << state << " " << readSymbol << " -> " 
        << newState << " " << writeSymbol << " " << directionToChar(direction);
    return oss.str();
}

bool TMRule::operator==(const TMRule& other) const {
    return state == other.state && readSymbol == other.readSymbol &&
           newState == other.newState && writeSymbol == other.writeSymbol &&
           direction == other.direction;
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
    if (is >> rule.state >> rule.readSymbol >> arrow >> rule.newState >> rule.writeSymbol >> dirChar) {
        rule.direction = charToDirection(dirChar);
    }
    return is;
}

TMRuleSet::TMRuleSet() = default;

TMRuleSet::TMRuleSet(const std::vector<TMRule>& rules) : rules(rules) {}

void TMRuleSet::addRule(const TMRule& rule) {
    rules.push_back(rule);
}

void TMRuleSet::addRule(const std::string& ruleStr) {
    std::istringstream iss(ruleStr);
    TMRule r;
    if (iss >> r) {
        rules.push_back(r);
    }
}

bool TMRuleSet::removeRule(const std::string& state, char symbol) {
    auto it = std::remove_if(rules.begin(), rules.end(), [&](const TMRule& r) { return r.matches(state, symbol); });
    if (it != rules.end()) {
        rules.erase(it, rules.end());
        return true;
    }
    return false;
}

void TMRuleSet::clear() {
    rules.clear();
}

const TMRule* TMRuleSet::findRule(const std::string& state, char symbol) const {
    for (const auto& rule : rules) {
        if (rule.matches(state, symbol)) {
            return &rule;
        }
    }
    return nullptr;
}

const std::vector<TMRule>& TMRuleSet::getRules() const {
    return rules;
}

size_t TMRuleSet::size() const {
    return rules.size();
}

bool TMRuleSet::operator==(const TMRuleSet& other) const {
    return rules == other.rules;
}

bool TMRuleSet::operator!=(const TMRuleSet& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const TMRuleSet& rs) {
    for (const auto& rule : rs.rules) {
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

TuringMachine::TuringMachine() : headIndex(0), blank('_'), halted(true), stepCount(0) {}

TuringMachine::TuringMachine(const std::string& initialTape, const TMRuleSet& rules,
                             const std::string& startState, char blank)
    : headIndex(0), blank(blank), ruleSet(rules), currentState(startState),
      halted(false), stepCount(0) {
    setTape(initialTape);
}

char TuringMachine::readCell() const {
    if (headIndex >= tape.size()) return blank;
    return tape[headIndex];
}

void TuringMachine::writeCell(char symbol) {
    if (headIndex >= tape.size()) {
        tape.resize(headIndex + 1, blank);
    }
    tape[headIndex] = symbol;
}

void TuringMachine::moveHead(Direction dir) {
    if (dir == Direction::Left) {
        if (headIndex == 0) {
            tape.push_front(blank);
        } else {
            headIndex--;
        }
    } else if (dir == Direction::Right) {
        headIndex++;
        if (headIndex >= tape.size()) {
            tape.push_back(blank);
        }
    }
}

bool TuringMachine::step() {
    if (halted) return false;

    char currentSymbol = readCell();
    const TMRule* rule = ruleSet.findRule(currentState, currentSymbol);

    if (!rule) {
        halted = true;
        return false;
    }

    writeCell(rule->getWriteSymbol());
    currentState = rule->getNewState();
    moveHead(rule->getDirection());
    stepCount++;

    return true;
}

void TuringMachine::run() {
    while (!halted) {
        step();
    }
}

void TuringMachine::setTape(const std::string& content) {
    tape.clear();
    for (char c : content) {
        tape.push_back(c);
    }
    if (tape.empty()) {
        tape.push_back(blank);
    }
    headIndex = 0;
    stepCount = 0;
    halted = false;
}

std::string TuringMachine::getTapeString() const {
    return std::string(tape.begin(), tape.end());
}

long TuringMachine::getHeadPosition() const {
    return static_cast<long>(headIndex);
}

std::string TuringMachine::getState() const { return currentState; }
bool TuringMachine::isHalted() const { return halted; }
size_t TuringMachine::getStepCount() const { return stepCount; }

std::ostream& operator<<(std::ostream& os, const TuringMachine& tm) {
    os << "State: " << tm.currentState << " | Steps: " << tm.stepCount << "\nTape: ";
    for (size_t i = 0; i < tm.tape.size(); ++i) {
        if (i == tm.headIndex) os << "[" << tm.tape[i] << "]";
        else os << tm.tape[i];
    }
    return os;
}