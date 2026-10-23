#include "Documents.hpp"
#include "People.hpp"


Document::Document(int n, const std::string& d) : number_(n), date_(d) {}
Document::~Document() {}

int Document::getNumber() const {
    return number_;
}

void Document::sign() {
    if (signed_) throw WarehouseException("Документ уже подписан");
    signed_ = true;
}

std::string Document::title() const {
    return "document";
}

Invoice::Invoice(int n, const std::string& d, double a) : Document(n, d), amount_(a) {
    if (a < 0) throw InvalidPriceException("Сумма счёта не может быть отрицательной");
}

double Invoice::total() const {
    return amount_ + tax();
}

double Invoice::tax() const {
    return amount_ * taxRate_;
}

void Invoice::applyDiscount(double percent) {
    if (percent < 0 || percent > 100) {
        throw InvalidDiscountException("Скидка должна быть от 0 до 100%");
    }
    amount_ *= (1.0 - percent / 100.0);
}

std::string Invoice::title() const {
    return "invoice";
}


Receipt::Receipt(int n, const std::string& d, bool p) : Document(n, d), paid_(p) {}

bool Receipt::isPaid() const {
    return paid_;
}

std::string Receipt::title() const {
    return "receipt";
}


Order::Order(int n, const std::string& d, const Customer* c, int items, const std::string status)
    : Document(n, d), customer_(c), items_count_(items), status_(status)
{
    if (items < 0) {
        throw InvalidQuantityException("Количество позиций не может быть отрицательным");
    }
}

int Order::getItemsCount() const {
    return items_count_;
}

void Order::cancel() {
    status_ = "cancelled";
}

std::string Order::title() const {
    if (customer_ == nullptr) return "order";
    return "order of customer " + customer_->getName();
}


PurchaseOrder::PurchaseOrder(int n, const std::string& d, int items, const Supplier* s)
    : Order(n, d, nullptr, items, "new"), supplier_(s) {}

const Supplier* PurchaseOrder::getSupplier() const {
    return supplier_;
}

std::string PurchaseOrder::title() const {
    return "purchase order from " + supplier_->getName();
}


ReturnNote::ReturnNote(int n, const std::string& d, const std::string& r)
    : Document(n, d), reason_(r) {}

std::string ReturnNote::getReason() const {
    return reason_;
}

void ReturnNote::approve(double amount) {
    if (amount < 0) {
        throw InvalidPriceException("Сумма возврата не может быть отрицательной");
    }
    refundAmount_ = amount;
}

std::string ReturnNote::title() const {
    return "return note";
}


Parcel::Parcel(double weightKg,
               const std::string& contents,
               const std::string& trackingCode,
               const Address& destination)
    : weight_kg_(weightKg),
      contents_(contents),
      trackingCode_(trackingCode),
      delivered_(false),
      destination_(destination)
{
    if (weightKg <= 0) {
        throw InvalidDimensionsException("Вес посылки должен быть положительным");
    }
    if (trackingCode.empty()) {
        throw std::invalid_argument("Трек-номер посылки не может быть пустым");
    }
}

std::string Parcel::trackingLabel() const {
    return "TRACK: " + trackingCode_ + " -> " + destination_.getAdress();
}

void Parcel::markDelivered() {
    delivered_ = true;
}

bool Parcel::isDelivered() const {
    return delivered_;
}

double Parcel::getWeight() const {
    return weight_kg_;
}