#pragma once

#include <iosfwd>
#include <string>

namespace sorting {

class Schoolboy {
public:
    Schoolboy(std::string name, int grade, double averageScore);

    const std::string& name() const noexcept;
    int grade() const noexcept;
    double averageScore() const noexcept;

    bool operator<(const Schoolboy& other) const noexcept;
    bool operator==(const Schoolboy& other) const noexcept;

    std::string describe() const;

private:
    std::string name_;
    int grade_;
    double averageScore_;
};

std::ostream& operator<<(std::ostream& stream, const Schoolboy& schoolboy);

}  