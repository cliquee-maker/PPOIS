#pragma once

#include <string>

class Tourist;

// Оценка с текстовым комментарием.
class Rating {
private:
    int value_;
    std::string comment_;
public:
    Rating(int value, const std::string& comment);
    bool isPositive() const;
    std::string stars() const;
    bool isBetterThan(const Rating& other) const;
};

// Отзыв туриста об услуге.
class Review {
private:
    const Tourist* author_;
    Rating rating_;
    std::string text_;
    std::string date_;
    int helpfulCount_ = 0;
    bool verified_ = false;
public:
    Review(const Tourist* author, const Rating& rating, const std::string& text,
           const std::string& date);
    std::string summary() const;
    void markHelpful();
    bool isVerified() const;
    bool matchesRating(int stars) const;
    void verify();
};

// Комментарий к отзыву.
class ReviewComment {
private:
    const Tourist* author_;
    std::string text_;
    std::string date_;
    bool reply_ = false;
public:
    ReviewComment(const Tourist* author, const std::string& text,
                  const std::string& date, bool reply);
    std::string preview(int maxLength) const;
    void edit(const std::string& text);
    bool isReply() const;
};
