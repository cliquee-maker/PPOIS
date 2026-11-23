#include "Schoolboy.h"

#include <ostream>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace sorting {

Schoolboy::Schoolboy(std::string name, int grade, double averageScore)
    : name_(std::move(name)), grade_(grade), averageScore_(averageScore) {
    if (name_.empty()) {
        throw std::invalid_argument("schoolboy name must not be empty");
    }
    if (grade_ < 1 || grade_ > 11) {
        throw std::invalid_argument("schoolboy grade must be within [1, 11]");
    }
    if (averageScore_ < 0.0 || averageScore_ > 5.0) {
        throw std::invalid_argument("schoolboy average score must be within [0, 5]");
    }
}

const std::string& Schoolboy::name() const noexcept {
    return name_;
}

int Schoolboy::grade() const noexcept {
    return grade_;
}

double Schoolboy::averageScore() const noexcept {
    return averageScore_;
}

bool Schoolboy::operator<(const Schoolboy& other) const noexcept {
    return std::tie(averageScore_, name_, grade_) <
           std::tie(other.averageScore_, other.name_, other.grade_);
}

bool Schoolboy::operator==(const Schoolboy& other) const noexcept {
    return std::tie(name_, grade_, averageScore_) ==
           std::tie(other.name_, other.grade_, other.averageScore_);
}

std::string Schoolboy::describe() const {
    return name_ + " (grade " + std::to_string(grade_) + ", avg " +
           std::to_string(averageScore_) + ")";
}

std::ostream& operator<<(std::ostream& stream, const Schoolboy& schoolboy) {
    return stream << schoolboy.describe();
}

}  