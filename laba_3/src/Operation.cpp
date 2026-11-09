#include "Operation.hpp"
#include <cctype>
#include <iomanip>
#include <sstream>


PromoCode::PromoCode(const std::string& code, double percent,
                     const DateRange& period, int usageLimit)
    : code_(code), percent_(percent), validPeriod_(period), usageLimit_(usageLimit) {
    if (code_.empty()) {
        throw InvalidPromoCodeException("Код промокода не может быть пустым.");
    }
    if (percent_ < 0.0 || percent_ > 100.0) {
        throw InvalidPromoCodeException("Некорректный процент промокода.");
    }
    if (usageLimit_ < 0) {
        throw InvalidPromoCodeException("Лимит использований не может быть отрицательным.");
    }
}

bool PromoCode::isValid(const std::string& date) const {
    return !isExhausted() && validPeriod_.contains(date);
}

double PromoCode::apply(double amount) const {
    if (amount < 0.0) {
        throw InvalidPriceException("Сумма не может быть отрицательной.");
    }
    return amount * (1.0 - percent_ / 100.0);
}

void PromoCode::consume() {
    if (usageLimit_ > 0) {
        --usageLimit_;
    }
}

bool PromoCode::isExhausted() const {
    return usageLimit_ <= 0;
}


PaymentCard::PaymentCard(const std::string& number, const std::string& holder,
                         const std::string& expiry)
    : number_(number), holder_(holder), expiry_(expiry) {}

bool PaymentCard::isValid() const {
    if (holder_.empty() || expiry_.empty()) {
        return false;
    }
    std::string digits;
    for (char c : number_) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digits.push_back(c);
        } else if (c != ' ' && c != '-') {
            return false;
        }
    }
    return digits.size() >= 12 && digits.size() <= 19;
}

std::string PaymentCard::maskedNumber() const {
    std::string digits;
    for (char c : number_) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digits.push_back(c);
        }
    }
    if (digits.size() <= 4) {
        return "****";
    }
    return "**** **** **** " + digits.substr(digits.size() - 4);
}

bool PaymentCard::belongsTo(const std::string& person) const {
    return holder_ == person;
}


Payment::Payment(Reservation* reservation, const PaymentCard& card, const Price& amount)
    : reservation_(reservation), card_(card), amount_(amount), status_("pending") {}

bool Payment::process() {
    if (reservation_ == nullptr) {
        throw PaymentFailedException("Платёж не привязан к бронированию.");
    }
    if (!card_.isValid()) {
        throw PaymentFailedException("Некорректные данные банковской карты.");
    }
    if (amount_.amount() <= 0.0) {
        throw PaymentFailedException("Сумма платежа должна быть положительной.");
    }
    status_ = "success";
    return true;
}

void Payment::fail(const std::string& reason) {
    status_ = reason.empty() ? "failed" : "failed: " + reason;
}

bool Payment::isSuccessful() const {
    return status_ == "success";
}

void Payment::refund(double sum) {
    if (status_ == "refunded") {
        throw RefundNotAllowedException("Платеж уже был возвращен.");
    }
    if (sum < 0.0 || sum > amount_.amount()) {
        throw RefundNotAllowedException("Некорректная сумма возврата.");
    }
    status_ = "refunded";
}


Transaction::Transaction(int id, const std::string& type, const Price& amount,
                         const std::string& timestamp)
    : id_(id), type_(type), amount_(amount), timestamp_(timestamp) {
    if (id_ < 0) {
        throw TravelException("Идентификатор транзакции не может быть отрицательным.");
    }
    if (type_.empty()) {
        throw TravelException("Тип транзакции не может быть пустым.");
    }
}

bool Transaction::isCredit() const {
    return type_ == "credit" || type_ == "deposit" || type_ == "payment";
}

std::string Transaction::receipt() const {
    std::ostringstream oss;
    oss << "Транзакция #" << id_ << " [" << type_ << "] "
        << std::fixed << std::setprecision(2) << amount_.amount()
        << ' ' << amount_.currency() << " в " << timestamp_;
    return oss.str();
}

void Transaction::reverse() {
    type_ = "reversal";
}

double Transaction::amount() const {
    return amount_.amount();
}


