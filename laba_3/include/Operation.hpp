#pragma once

#include <cstddef>
#include <string>
#include "except.hpp"
#include "Basic.hpp"

class Reservation;

class PromoCode {
private:
    std::string code_;
    double percent_;
    DateRange validPeriod_;
    int usageLimit_;
public:
    PromoCode(const std::string& code, double percent,
              const DateRange& period, int usageLimit);
    bool isValid(const std::string& date) const;
    double apply(double amount) const;
    void consume();
    bool isExhausted() const;
};

class PaymentCard {
private:
    std::string number_;
    std::string holder_;
    std::string expiry_;
public:
    PaymentCard(const std::string& number, const std::string& holder,
                const std::string& expiry);
    bool isValid() const;
    std::string maskedNumber() const;
    bool belongsTo(const std::string& person) const;
};

class Payment {
private:
    Reservation* reservation_;
    PaymentCard card_;
    Price amount_;
    std::string status_;
public:
    Payment(Reservation* reservation, const PaymentCard& card, const Price& amount);
    bool process();
    void fail(const std::string& reason);
    bool isSuccessful() const;
    void refund(double sum);
};

class Transaction {
private:
    int id_;
    std::string type_;
    Price amount_;
    std::string timestamp_;
public:
    Transaction(int id, const std::string& type, const Price& amount,
                const std::string& timestamp);
    bool isCredit() const;
    std::string receipt() const;
    void reverse();
};

class Refund {
private:
    const Transaction* original_;
    Price amount_;
    std::string reason_;
    bool approved_ = false;
public:
    Refund(const Transaction* original, const Price& amount,
           const std::string& reason);
    bool approve();
    bool isFullRefund() const;
    double fee() const;
};

class Notification {
private:
    std::string recipient_;
    std::string message_;
    std::string channel_;
    bool sent_ = false;
public:
    Notification(const std::string& recipient, const std::string& message,
                 const std::string& channel);
    bool send();
    void markRead();
    std::string preview(std::size_t maxLength) const;
};
