#pragma once

#include <string>
#include "except.hpp"
#include "Basic.hpp"

class Customer;
class Supplier;


class Document {
protected:
    int number_;
    std::string date_;
    bool signed_ = false;
public:
    Document(int n, const std::string& d);
    virtual ~Document();
    int getNumber() const;
    void sign();
    virtual std::string title() const;
};

class Invoice : virtual public Document {
    double amount_;
    double taxRate_ = 0.2;
public:
    Invoice(int n, const std::string& d, double a);
    double total() const;
    double tax() const;
    void applyDiscount(double percent);
    std::string title() const override;
};

class Receipt : virtual public Document {
    bool paid_;
public:
    Receipt(int n, const std::string& d, bool p);
    bool isPaid() const;
    std::string title() const override;
};

class Order : public Document {
    const Customer* customer_;
    int items_count_;
    std::string status_;
public:
    Order(int n, const std::string& d, const Customer* c, int items, const std::string status);
    int getItemsCount() const;
    void cancel();
    std::string title() const override;
};


class PurchaseOrder : public Order {
    const Supplier* supplier_;               
public:
    PurchaseOrder(int n, const std::string& d, int items, const Supplier* s);
    const Supplier* getSupplier() const;
    std::string title() const override;
};

class ReturnNote : public Document {
    std::string reason_;
    double refundAmount_ = 0.0;
public:
    ReturnNote(int n, const std::string& d, const std::string& r);
    std::string getReason() const;
    void approve(double amount);
    std::string title() const override;
};

class Parcel {
private:
    double weight_kg_;
    std::string contents_;
    std::string trackingCode_;
    bool delivered_;
    Address destination_;                   

public:
    Parcel(double weightKg,
           const std::string& contents,
           const std::string& trackingCode,
           const Address& destination);
    std::string trackingLabel() const;
    void markDelivered();
    bool isDelivered() const;
    double getWeight() const; 
};