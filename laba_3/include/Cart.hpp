#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "except.hpp"
#include "Basic.hpp"

class TravelService;
class Tourist;

class CartItem {
private:
    const TravelService* service_;
    int quantity_;
    Price unitPrice_;
public:
    CartItem(const TravelService* service, int quantity, const Price& unitPrice);
    double subtotal() const;
    void increase(int n);
    bool isSameService(const TravelService& service) const;
};

class Cart {
private:
    const Tourist* owner_;
    std::vector<CartItem> items_;
    std::string promoCode_;
public:
    explicit Cart(const Tourist* owner);
    void addItem(const TravelService* service, const Price& price);
    void removeItem(const TravelService& service);
    double total() const;
    bool isEmpty() const;
    void clear();
    void applyPromo(const std::string& code);
};

class WishList {
private:
    const Tourist* owner_;
    std::vector<const TravelService*> items_;
public:
    explicit WishList(const Tourist* owner);
    void add(const TravelService* service);
    bool contains(const TravelService& service) const;
    bool moveToCart(Cart& cart, const Price& price);
    std::size_t size() const;
};
