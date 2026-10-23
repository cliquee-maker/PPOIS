#pragma once

#include <string>
#include <vector>
#include "except.hpp"
#include "Basic.hpp" 

class Customer;
class StorageItem;

class PromoCode {
    std::string code_;
    double discount_percent_;
    bool active_;
    int usesLeft_ = 10;
public:
    PromoCode(const std::string& code, double discount);
    void use();
    double apply(double price) const;
    void deactivate();
    bool isActive() const { return active_; }
};

class Cart {
    const Customer* customer_;
    std::vector<const StorageItem*> items_;
    const PromoCode* promo_;
public:
    explicit Cart(const Customer* customer);
    void addItem(const StorageItem* item);
    void removeItem(const StorageItem* item);
    void applyPromoCode(const PromoCode* promo);
    size_t getItemsCount() const;
    void clear();
    bool contains(const StorageItem* item) const;
};

class OrderItem {
    const StorageItem* item_;
    int quantity_;
    Price price_;
public:
    OrderItem(const StorageItem* item, int qty, const Price& price);
    double getTotal() const;
    void changeQuantity(int qty);
};

class Payment {
protected:
    double amount_;
    bool success_;
    std::string refundDate_;
public:
    explicit Payment(double amount);
    virtual ~Payment();
    virtual bool processPayment() = 0;
    void refund(const std::string& date);
};

class CardPayment : public Payment {
    std::string card_number_;
    std::string ownerName_ = "CARD HOLDER";
public:
    CardPayment(double amount, const std::string& cardNum);
    bool processPayment() override;
    bool isCardValid() const;
};

class CashPayment : public Payment {
    double cash_tendered_;
public:
    CashPayment(double amount, double tendered);
    bool processPayment() override;
    double getChange() const;
};