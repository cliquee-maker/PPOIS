#include "Cart.hpp"
#include "Items.hpp"
#include "People.hpp"
#include <algorithm>
#include <cctype>


PromoCode::PromoCode(const std::string& code, double discount)
    : code_(code), discount_percent_(discount), active_(true)
{
    if (code.empty()) {
        throw InvalidPromoCodeException("Промокод не может быть пустым");
    }
    if (discount < 0 || discount > 100) {
        throw InvalidPromoCodeException("Скидка должна быть от 0 до 100 процентов");
    }
}

double PromoCode::apply(double price) const {
    if (!active_ || discount_percent_ < 0) return price;
    return price * (1.0 - discount_percent_ / 100.0);
}

void PromoCode::deactivate() {
    active_ = false;
}

void PromoCode::use() {
    if (!active_ || usesLeft_ <= 0) {
        throw InvalidPromoCodeException("Промокод " + code_ + " недействителен");
    }
    --usesLeft_;
    if (usesLeft_ == 0) active_ = false;
}


Cart::Cart(const Customer* customer) : customer_(customer), promo_(nullptr) {}

void Cart::addItem(const StorageItem* item) {
    if (item == nullptr) {
        throw InvalidQuantityException("Нельзя добавить пустой товар");
    }
    items_.push_back(item);
}

void Cart::removeItem(const StorageItem* item) {
    auto it = std::find(items_.begin(), items_.end(), item);
    if (it == items_.end()) {
        throw OutOfStockException("Такого товара нет в корзине");
    }
    items_.erase(it);
}

void Cart::applyPromoCode(const PromoCode* promo) {
    if (promo == nullptr || !promo->isActive()) {
        throw InvalidPromoCodeException("Промокод недействителен");
    }
    promo_ = promo;
}

size_t Cart::getItemsCount() const {
    return items_.size();
}

void Cart::clear() {
    items_.clear();
    promo_ = nullptr;
}

bool Cart::contains(const StorageItem* item) const {
    return std::find(items_.begin(), items_.end(), item) != items_.end();
}


OrderItem::OrderItem(const StorageItem* item, int qty, const Price& price)
    : item_(item), quantity_(qty), price_(price)
{
    if (item == nullptr) {
        throw InvalidQuantityException("Позиция заказа должна ссылаться на товар");
    }
    if (qty <= 0) {
        throw InvalidQuantityException("Количество товара должно быть положительным");
    }
}

double OrderItem::getTotal() const {
    return quantity_ * price_.getPriceWithSale(0);
}

void OrderItem::changeQuantity(int qty) {
    if (qty <= 0) {
        throw InvalidQuantityException("Количество должно быть больше нуля");
    }
    quantity_ = qty;
}


Payment::Payment(double amount) : amount_(amount), success_(false) {}
Payment::~Payment() {}

void Payment::refund(const std::string& date) {
    if (!success_) {
        throw PaymentFailedException("Нельзя вернуть неоплаченный платёж");
    }
    success_ = false;
    refundDate_ = date;
}


CardPayment::CardPayment(double amount, const std::string& cardNum)
    : Payment(amount), card_number_(cardNum) {}

bool CardPayment::processPayment() {
    if (card_number_.empty()) {
        throw PaymentFailedException("Номер карты не указан");
    }
    if (!isCardValid()) {
        throw PaymentFailedException("Номер карты некорректен");
    }
    success_ = amount_ > 0;
    return success_;
}

bool CardPayment::isCardValid() const {
    if (ownerName_.empty()) return false;
    int sum = 0, digits = 0;
    bool doubleIt = false;
    for (auto it = card_number_.rbegin(); it != card_number_.rend(); ++it) {
        if (*it == ' ') continue;
        if (!std::isdigit(static_cast<unsigned char>(*it))) return false;
        int d = *it - '0';
        if (doubleIt) { d *= 2; if (d > 9) d -= 9; }
        sum += d;
        doubleIt = !doubleIt;
        ++digits;
    }
    return digits >= 13 && digits <= 19 && sum % 10 == 0;
}

CashPayment::CashPayment(double amount, double tendered)
    : Payment(amount), cash_tendered_(tendered) {}

bool CashPayment::processPayment() {
    if (amount_ <= 0 || cash_tendered_ < 0) {
        success_ = false;
        throw PaymentFailedException("Некорректная сумма оплаты");
    }
    if (cash_tendered_ < amount_) {
        success_ = false;
        throw PaymentFailedException("Внесённых денег недостаточно");
    }
    success_ = true;
    return true;
}

double CashPayment::getChange() const {
    if (cash_tendered_ < amount_) {
        throw PaymentFailedException("Недостаточно наличных");
    }
    return cash_tendered_ - amount_;
}