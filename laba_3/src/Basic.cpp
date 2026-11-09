#include "Basic.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace {

constexpr std::time_t SECONDS_PER_DAY = 24 * 60 * 60;

std::time_t parseDate(const std::string& date) {
    std::tm tm{};
    std::istringstream iss(date);
    iss >> std::get_time(&tm, "%Y-%m-%d");
    if (iss.fail()) {
        throw InvalidDateRangeException("Некорректная дата: " + date);
    }
    tm.tm_hour = 12;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_isdst = -1;
    const std::time_t result = std::mktime(&tm);
    if (result == static_cast<std::time_t>(-1)) {
        throw InvalidDateRangeException("Не удалось разобрать дату: " + date);
    }
    return result;
}

std::string formatDate(std::time_t value) {
    std::tm* tm = std::localtime(&value);
    if (tm == nullptr) {
        throw InvalidDateRangeException("Не удалось преобразовать дату.");
    }
    char buffer[11] = {};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm);
    return std::string(buffer);
}

} 


Currency::Currency(const std::string& code, const std::string& symbol, double rateToUsd)
    : code_(code), symbol_(symbol), rateToUsd_(rateToUsd) {
    if (code_.empty()) {
        throw InvalidPriceException("Код валюты не может быть пустым.");
    }
    if (rateToUsd_ <= 0.0) {
        throw InvalidPriceException("Курс валюты должен быть положительным.");
    }
}

double Currency::convertFromUsd(double usd) const {
    return usd * rateToUsd_;
}

double Currency::convertToUsd(double amount) const {
    return amount / rateToUsd_;
}

std::string Currency::format(double amount) const {
    std::ostringstream oss;
    oss << symbol_ << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}

bool Currency::isSame(const Currency& other) const {
    return code_ == other.code_;
}


Price::Price(double p, const std::string& currency)
    : price_(p), currency_(currency) {
    if (p < 0.0) {
        throw InvalidPriceException("Цена не может быть отрицательной.");
    }
    if (currency_.empty()) {
        throw InvalidPriceException("Валюта цены не может быть пустой.");
    }
}

double Price::getPriceWithTax(int taxPercent) const {
    if (taxPercent < 0) {
        throw InvalidPriceException("Налоговый процент не может быть отрицательным.");
    }
    return price_ * (1.0 + taxPercent / 100.0);
}

double Price::getPriceWithDiscount(int salePercent) const {
    if (salePercent < 0 || salePercent > 100) {
        throw InvalidPriceException("Некорректный процент скидки.");
    }
    return price_ * (1.0 - salePercent / 100.0);
}

Price Price::convertTo(double rate, const std::string& currency) const {
    if (rate <= 0.0) {
        throw InvalidPriceException("Курс конвертации должен быть больше нуля.");
    }
    return Price(price_ * rate, currency);
}

Price Price::add(const Price& other) const {
    if (currency_ != other.currency_) {
        throw InvalidPriceException("Сложение цен в разных валютах недопустимо.");
    }
    return Price(price_ + other.price_, currency_);
}

Price Price::subtract(const Price& other) const {
    if (currency_ != other.currency_) {
        throw InvalidPriceException("Вычитание цен в разных валютах недопустимо.");
    }
    if (price_ < other.price_) {
        throw InvalidPriceException("Итоговая цена не может быть отрицательной.");
    }
    return Price(price_ - other.price_, currency_);
}

bool Price::isAffordable(double budget) const {
    return price_ <= budget;
}

double Price::amount() const {
    return price_;
}

std::string Price::currency() const {
    return currency_;
}


DateRange::DateRange(const std::string& start, const std::string& end)
    : startDate_(start), endDate_(end) {
    if (parseDate(startDate_) > parseDate(endDate_)) {
        throw InvalidDateRangeException("Дата начала не может быть позже даты окончания.");
    }
}

int DateRange::nights() const {
    const std::time_t start = parseDate(startDate_);
    const std::time_t end = parseDate(endDate_);
    return static_cast<int>((end - start) / SECONDS_PER_DAY);
}

bool DateRange::overlaps(const DateRange& other) const {
    const std::time_t start = parseDate(startDate_);
    const std::time_t end = parseDate(endDate_);
    const std::time_t otherStart = parseDate(other.startDate_);
    const std::time_t otherEnd = parseDate(other.endDate_);
    return !(end < otherStart || start > otherEnd);
}

bool DateRange::contains(const std::string& date) const {
    const std::time_t value = parseDate(date);
    return value >= parseDate(startDate_) && value <= parseDate(endDate_);
}

void DateRange::extend(int days) {
    if (days < 0) {
        throw InvalidDateRangeException("Нельзя продлить диапазон на отрицательное число дней.");
    }
    const std::time_t newEnd = parseDate(endDate_) + static_cast<std::time_t>(days) * SECONDS_PER_DAY;
    endDate_ = formatDate(newEnd);
}

std::string DateRange::getStart() const {
    return startDate_;
}

std::string DateRange::getEnd() const {
    return endDate_;
}


ContactInfo::ContactInfo(const std::string& phone, const std::string& email)
    : phone_(phone), email_(email) {}

bool ContactInfo::isValidEmail() const {
    return email_.find('@') != std::string::npos
        && email_.find('.') != std::string::npos;
}

bool ContactInfo::isValidPhone() const {
    return !phone_.empty() && phone_.size() >= 7;
}

void ContactInfo::setPhone(const std::string& phone) {
    if (phone.empty()) {
        throw InvalidPassportException("Телефон не может быть пустым.");
    }
    phone_ = phone;
}

void ContactInfo::setEmergencyContact(const std::string& phone) {
    if (phone.empty()) {
        throw InvalidPassportException("Экстренный телефон не может быть пустым.");
    }
    emergencyPhone_ = phone;
}

std::string ContactInfo::maskedPhone() const {
    if (phone_.size() <= 4) {
        return "****";
    }
    return phone_.substr(0, 3) + "***" + phone_.substr(phone_.size() - 2);
}


Location::Location(const std::string& country, const std::string& city,
                   const std::string& airportCode)
    : country_(country), city_(city), airportCode_(airportCode) {
    if (country_.empty() || city_.empty() || airportCode_.empty()) {
        throw TravelException("Страна, город и код аэропорта обязательны.");
    }
}

std::string Location::getFullLocation() const {
    return city_ + ", " + country_ + " (" + airportCode_ + ")";
}

std::string Location::getCountry() const {
    return country_;
}

std::string Location::getAirportCode() const {
    return airportCode_;
}

bool Location::sameCountry(const Location& other) const {
    return country_ == other.country_;
}

double Location::distanceTo(const Location& other) const {
    if (airportCode_ == other.airportCode_) {
        return 0.0;
    }
    if (country_ == other.country_) {
        return 500.0;
    }
    return 3000.0;
}


BaggageAllowance::BaggageAllowance(double weight, int pieces, double volume)
    : maxWeightKg_(weight), maxPieces_(pieces), maxVolumeM3_(volume) {
    if (weight < 0.0 || pieces < 0 || volume < 0.0) {
        throw ExcessBaggageException("Параметры багажа не могут быть отрицательными.");
    }
}

double BaggageAllowance::getMaxWeight() const {
    return maxWeightKg_;
}

int BaggageAllowance::getMaxPieces() const {
    return maxPieces_;
}

bool BaggageAllowance::fits(double weight, double volume) const {
    return weight >= 0.0 && volume >= 0.0
        && weight <= maxWeightKg_ && volume <= maxVolumeM3_;
}

double BaggageAllowance::excessWeight(double weight) const {
    return weight > maxWeightKg_ ? weight - maxWeightKg_ : 0.0;
}

bool BaggageAllowance::requiresExtraFee(double weight) const {
    return weight > maxWeightKg_;
}


TourCategory::TourCategory(int id, const std::string& name, int ageLimit)
    : id_(id), name_(name), ageLimit_(ageLimit) {
    if (id_ < 0) {
        throw TravelException("Идентификатор категории не может быть отрицательным.");
    }
    if (ageLimit_ < 0 || ageLimit_ > 99) {
        throw TravelException("Некорректный возрастной лимит категории.");
    }
}

std::string TourCategory::getCategoryName() const {
    return name_;
}

int TourCategory::getAgeLimit() const {
    return ageLimit_;
}

bool TourCategory::allowedFor(int age) const {
    return age >= ageLimit_;
}

bool TourCategory::matches(const TourCategory& other) const {
    return id_ == other.id_;
}

std::string TourCategory::describe() const {
    return "Категория: " + name_ + " (" + std::to_string(ageLimit_) + "+)";
}


TravelAgent::TravelAgent(int i, const std::string& n, int exp)
    : id_(i), name_(n), experienceYears_(exp) {
    if (experienceYears_ < 0) {
        throw TravelException("Опыт агента не может быть отрицательным.");
    }
}

std::string TravelAgent::getAgentInfo() const {
    return "Агент: " + name_ + " (агентство: " + agencyName_ + ")";
}

int TravelAgent::experienceYears() const {
    return experienceYears_;
}

void TravelAgent::setAgencyName(const std::string& agency) {
    if (agency.empty()) {
        throw TravelException("Название агентства не может быть пустым.");
    }
    agencyName_ = agency;
}

bool TravelAgent::isTopSeller(int threshold) const {
    return experienceYears_ >= threshold;
}

bool TravelAgent::canHandle(const TourCategory& category) const {
    return category.getAgeLimit() < 18 || experienceYears_ >= 2;
}


TourOperator::TourOperator(int i, const std::string& n, const Location& loc)
    : id_(i), name_(n), hqLocation_(loc), foundedYear_(2000) {}

std::string TourOperator::getOperatorInfo() const {
    return "Оператор: " + name_ + ", офис: " + hqLocation_.getFullLocation();
}

void TourOperator::setFoundedYear(int year) {
    if (year < 1900 || year > 2100) {
        throw InvalidDateRangeException("Некорректный год основания.");
    }
    foundedYear_ = year;
}

int TourOperator::yearsOnMarket(int currentYear) const {
    if (currentYear < foundedYear_) {
        throw InvalidDateRangeException("Текущий год раньше года основания.");
    }
    return currentYear - foundedYear_;
}

bool TourOperator::isVeteran(int threshold) const {
    return yearsOnMarket(2026) >= threshold;
}

bool TourOperator::serves(const Location& destination) const {
    return hqLocation_.sameCountry(destination);
}

