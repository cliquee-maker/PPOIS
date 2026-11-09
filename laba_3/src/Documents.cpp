#include "Documents.hpp"
#include <iomanip>
#include <sstream>


TravelDocument::TravelDocument(int n, const std::string& d)
    : documentNumber_(n), issueDate_(d), verified_(false) {
    if (documentNumber_ < 0) {
        throw TravelException("Номер документа не может быть отрицательным.");
    }
}

TravelDocument::~TravelDocument() = default;

int TravelDocument::getNumber() const {
    return documentNumber_;
}

void TravelDocument::verify() {
    verified_ = true;
}

bool TravelDocument::isVerified() const {
    return verified_;
}

bool TravelDocument::isValidOn(const std::string&) const {
    return verified_;
}

std::string TravelDocument::title() const {
    return "Документ #" + std::to_string(documentNumber_);
}



Invoice::Invoice(int n, const std::string& d, double a)
    : TravelDocument(n, d), amount_(a), taxRate_(0.1), paid_(false) {
    if (amount_ < 0.0) {
        throw InvalidPriceException("Сумма счёта не может быть отрицательной.");
    }
}

double Invoice::total() const {
    return amount_ + tax();
}

double Invoice::tax() const {
    return amount_ * taxRate_;
}

void Invoice::applyDiscount(double percent) {
    if (percent < 0.0 || percent > 100.0) {
        throw InvalidPriceException("Некорректный процент скидки.");
    }
    amount_ *= (1.0 - percent / 100.0);
}

void Invoice::addLineItem(double amount) {
    if (amount < 0.0) {
        throw InvalidPriceException("Сумма позиции не может быть отрицательной.");
    }
    amount_ += amount;
}

void Invoice::markPaid() {
    paid_ = true;
}

bool Invoice::isPaid() const {
    return paid_;
}

std::string Invoice::title() const {
    std::ostringstream oss;
    oss << "Счёт #" << documentNumber_ << " на " << std::fixed
        << std::setprecision(2) << total();
    return oss.str();
}



TravelVoucher::TravelVoucher(int n, const std::string& d, bool r)
    : TravelDocument(n, d), redeemed_(r) {}

bool TravelVoucher::isRedeemed() const {
    return redeemed_;
}

bool TravelVoucher::canRedeem() const {
    return !redeemed_ && verified_;
}

void TravelVoucher::redeem() {
    if (!canRedeem()) {
        throw ReservationException("Путёвка не может быть погашена.");
    }
    redeemed_ = true;
}

std::string TravelVoucher::title() const {
    return "Путёвка #" + std::to_string(documentNumber_);
}



ETicket::ETicket(int n, const std::string& d, const std::string& flightCode,
                 const std::string& seat)
    : TravelDocument(n, d), flightCode_(flightCode), seat_(seat) {
    if (flightCode_.empty()) {
        throw TravelException("Код рейса не может быть пустым.");
    }
}

void ETicket::reissue(const std::string& newFlightCode, const std::string& newSeat) {
    if (newFlightCode.empty()) {
        throw TravelException("Новый код рейса не может быть пустым.");
    }
    flightCode_ = newFlightCode;
    seat_ = newSeat;
}

bool ETicket::isBoardingReady() const {
    return verified_ && !seat_.empty();
}

std::string ETicket::title() const {
    return "Электронный билет #" + std::to_string(documentNumber_)
        + " (" + flightCode_ + ")";
}



VisaDocument::VisaDocument(int n, const std::string& d, const std::string& country,
                           const DateRange& validity)
    : TravelDocument(n, d), country_(country), validity_(validity) {
    if (country_.empty()) {
        throw VisaRequiredException("Страна визы не может быть пустой.");
    }
}

bool VisaDocument::covers(const DateRange& trip) const {
    return validity_.getStart() <= trip.getStart()
        && validity_.getEnd() >= trip.getEnd();
}

bool VisaDocument::isExpired(const std::string& date) const {
    return date > validity_.getEnd();
}

std::string VisaDocument::title() const {
    return "Виза #" + std::to_string(documentNumber_) + " (" + country_ + ")";
}



BookingConfirmation::BookingConfirmation(int n, const std::string& d,
                                         const Tourist* t, int count,
                                         const std::string& status)
    : TravelDocument(n, d), tourist_(t), servicesCount_(count), status_(status) {
    if (servicesCount_ < 0) {
        throw ReservationException("Количество услуг не может быть отрицательным.");
    }
}

int BookingConfirmation::getServicesCount() const {
    return servicesCount_;
}

void BookingConfirmation::cancel() {
    status_ = "cancelled";
}

void BookingConfirmation::confirm() {
    status_ = "confirmed";
}

bool BookingConfirmation::isActive() const {
    return status_ == "confirmed" || status_ == "pending";
}

std::string BookingConfirmation::title() const {
    const std::string who = tourist_ != nullptr
        ? tourist_->getName()
        : "неизвестный турист";
    return "Подтверждение #" + std::to_string(documentNumber_)
        + " для " + who + " (" + status_ + ")";
}



InsurancePolicy::InsurancePolicy(int n, const std::string& d,
                                 const std::string& coverage, double coverageAmount)
    : TravelDocument(n, d), coverageType_(coverage), coverageAmount_(coverageAmount) {
    if (coverageType_.empty()) {
        throw TravelException("Тип покрытия не может быть пустым.");
    }
    if (coverageAmount_ < 0.0) {
        throw InvalidPriceException("Размер покрытия не может быть отрицательным.");
    }
}

std::string InsurancePolicy::getCoverageType() const {
    return coverageType_;
}

void InsurancePolicy::approveClaim(double amount) {
    if (amount < 0.0) {
        throw InvalidPriceException("Сумма выплаты не может быть отрицательной.");
    }
    if (amount > coverageAmount_) {
        throw PaymentFailedException("Сумма выплаты превышает покрытие.");
    }
    coverageAmount_ -= amount;
}

bool InsurancePolicy::covers(double damage) const {
    return damage >= 0.0 && damage <= coverageAmount_;
}

bool InsurancePolicy::isPremium() const {
    return coverageAmount_ >= 100000.0;
}

std::string InsurancePolicy::title() const {
    return "Полис #" + std::to_string(documentNumber_)
        + " (" + coverageType_ + ")";
}