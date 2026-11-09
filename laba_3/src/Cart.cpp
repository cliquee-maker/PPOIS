#include "Cart.hpp"
#include <algorithm>

CartItem::CartItem(const TravelService* service, int quantity, const Price& unitPrice)
    : service_(service), quantity_(quantity), unitPrice_(unitPrice) {
    if (service_ == nullptr) {
        throw ReservationException("Услуга в корзине не может быть пустой.");
    }
    if (quantity_ <= 0) {
        throw ReservationException("Количество услуг должно быть положительным.");
    }
}

double CartItem::subtotal() const {
    return unitPrice_.amount() * static_cast<double>(quantity_);
}

void CartItem::increase(int n) {
    if (n <= 0) {
        throw ReservationException("Увеличение количества должно быть положительным.");
    }
    quantity_ += n;
}

bool CartItem::isSameService(const TravelService& service) const {
    return service_ == &service;
}

Cart::Cart(const Tourist* owner)
    : owner_(owner), promoCode_() {}

void Cart::addItem(const TravelService* service, const Price& price) {
    if (service == nullptr) {
        throw ReservationException("Нельзя добавить пустую услугу в корзину.");
    }
    for (CartItem& item : items_) {
        if (item.isSameService(*service)) {
            item.increase(1);
            return;
        }
    }
    items_.emplace_back(service, 1, price);
}

void Cart::removeItem(const TravelService& service) {
    items_.erase(std::remove_if(items_.begin(), items_.end(),
                                [&service](const CartItem& item) {
                                    return item.isSameService(service);
                                }),
                 items_.end());
}

double Cart::total() const {
    double sum = 0.0;
    for (const CartItem& item : items_) {
        sum += item.subtotal();
    }
    if (promoCode_ == "SAVE10") {
        return sum * 0.9;
    }
    if (promoCode_ == "SAVE20") {
        return sum * 0.8;
    }
    if (!promoCode_.empty()) {
        throw InvalidPromoCodeException("Неизвестный промокод: " + promoCode_);
    }
    return sum;
}

bool Cart::isEmpty() const {
    return items_.empty();
}

void Cart::clear() {
    items_.clear();
    promoCode_.clear();
}

void Cart::applyPromo(const std::string& code) {
    if (code.empty()) {
        promoCode_.clear();
        return;
    }
    if (code != "SAVE10" && code != "SAVE20") {
        throw InvalidPromoCodeException("Неизвестный промокод: " + code);
    }
    promoCode_ = code;
}


WishList::WishList(const Tourist* owner)
    : owner_(owner), items_() {}

void WishList::add(const TravelService* service) {
    if (service == nullptr) {
        throw ReservationException("Нельзя добавить пустую услугу в список желаний.");
    }
    if (!contains(*service)) {
        items_.push_back(service);
    }
}

bool WishList::contains(const TravelService& service) const {
    return std::find(items_.begin(), items_.end(), &service) != items_.end();
}

bool WishList::moveToCart(Cart& cart, const Price& price) {
    if (items_.empty()) {
        return false;
    }
    const TravelService* service = items_.front();
    cart.addItem(service, price);
    items_.erase(items_.begin());
    return true;
}

std::size_t WishList::size() const {
    return items_.size();
}
