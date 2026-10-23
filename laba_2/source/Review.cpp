#include "Review.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>


Review::Review(const Book* book, const Customer* customer, int rating, const std::string& comment)
    : book_(book), customer_(customer), rating_(rating), comment_(comment)
{
    if (rating < 1 || rating > 5) {
        throw InvalidRatingException("Оценка в отзыве должна быть от 1 до 5");
    }
}

int Review::getRating() const {
    return rating_;
}

std::string Review::getComment() const {
    return comment_;
}

void Review::like() {
    ++likes_;
}

bool Review::isPositive() const {
    return rating_ >= 4;
}


Notification::Notification(const std::string& recipient, const std::string& msg)
    : recipient_(recipient), message_(msg) {}

Notification::~Notification() {}


EmailNotification::EmailNotification(const std::string& email, const std::string& subject, const std::string& msg)
    : Notification(email, msg), subject_(subject) {}

void EmailNotification::send() {
    if (!isValidEmail()) throw WarehouseException("Некорректный email: " + recipient_);
    std::cout << "[Email to " << recipient_ << "] Subject: " << subject_
              << "\nMessage: " << message_ << std::endl;
}

bool EmailNotification::isValidEmail() const {
    auto at = recipient_.find('@');
    return at != std::string::npos && at > 0 &&
           recipient_.find('.', at) != std::string::npos;
}


SMSNotification::SMSNotification(const std::string& phone, const std::string& msg)
    : Notification(phone, msg), phone_(phone) {}

void SMSNotification::send() {
    if (!isValidPhone()) throw WarehouseException("Некорректный номер: " + phone_);
    std::cout << "[SMS to " << phone_ << "] Message: " << message_ << std::endl;
}

bool SMSNotification::isValidPhone() const {
    if (phone_.size() < 11 || phone_[0] != '+') return false;
    return std::all_of(phone_.begin() + 1, phone_.end(),
                       [](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
}