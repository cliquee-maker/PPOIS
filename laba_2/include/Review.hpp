#pragma once
#include <string>
#include "except.hpp"

class Book;
class Customer;

class Review {
    const Book* book_;
    const Customer* customer_;
    int rating_;
    std::string comment_;
    int likes_ = 0;
public:
    Review(const Book* book, const Customer* customer, int rating, const std::string& comment);
    int getRating() const;
    std::string getComment() const;
    void like();
    bool isPositive() const;
};

class Notification {
protected:
    std::string recipient_;
    std::string message_;
public:
    Notification(const std::string& recipient, const std::string& msg);
    virtual ~Notification();
    virtual void send() = 0;
};

class EmailNotification : public Notification {
    std::string subject_;
public:
    EmailNotification(const std::string& email, const std::string& subject, const std::string& msg);
    void send() override;
    bool isValidEmail() const;
};

class SMSNotification : public Notification {
    std::string phone_;
public:
    SMSNotification(const std::string& phone, const std::string& msg);
    void send() override;
    bool isValidPhone() const;
};