#include "Basic.hpp"


Address::Address(const std::string country, const std::string& c, const std::string& s, const std::string& z)
    : city_(c), street_(s), zip_(z), country_(country) {}

std::string Address::getAdress() const {
    return country_ + ", " + city_ + ", " + street_ + " " + zip_;
}

std::string Address::getCountry() const {
    return country_;
}


Price::Price(double p, const std::string currency) : price_(p), currency_(currency) {
    if (p < 0) {
        throw InvalidPriceException("Цена не может быть отрицательной!");
    }
    if (currency.empty()) {
        throw InvalidPriceException("Валюта не указана");
    }
}

double Price::getPriceWithNDS(int NDS) const {
    return price_ * (100 + NDS) / 100.0;
}

double Price::getPriceWithSale(int Sale) const {
    if (Sale < 0 || Sale > 100) {
        throw InvalidDiscountException("Скидка должна быть от 0 до 100%");
    }
    return price_ * (100 - Sale) / 100.0;
}

Price Price::convertTo(double rate, const std::string& currency) const {
    if (rate <= 0) {
        throw InvalidPriceException("Курс валюты должен быть больше нуля");
    }
    return Price(price_ * rate, currency);
}

Price Price::add(const Price& other) const {
    if (currency_ != other.currency_) {
        throw InvalidPriceException("Нельзя складывать цены в разных валютах");
    }
    return Price(price_ + other.price_, currency_);
}


Dimensions::Dimensions(double h, double l, double w, double weight)
    : high_(h), length_(l), width_(w), weight_(weight) {
    if (h <= 0 || l <= 0 || w <= 0 || weight <= 0) {
        throw InvalidDimensionsException("Все габариты и вес должны быть > 0");
    }
}

double Dimensions::getWeight() const { return weight_; }
double Dimensions::getWidth() const { return width_; }
double Dimensions::getHigh() const { return high_; }
double Dimensions::getLength() const { return length_; }

double Dimensions::getVolume() const {
    return high_ * length_ * width_;
}

bool Dimensions::fitsIn(double cell) const {
    return getVolume() <= cell;
}


Genre::Genre(int id, const std::string& name, int ageLimit)
    : id_(id), name_(name), ageLimit_(ageLimit) {
    if (ageLimit < 0) {
        throw AgeRestrictionException("Возрастное ограничение не может быть отрицательным");
    }
}

std::string Genre::getGenre() const { return name_; }

int Genre::getAge() const { return ageLimit_; }

bool Genre::allowedFor(int age) const {
    if (age < 0) throw AgeRestrictionException("Возраст не может быть отрицательным");
    return age >= ageLimit_;
}


Author::Author(int i, const std::string& n, int b)
    : id_(i), name_(n), birthYear_(b) {}

std::string Author::getAuthor() const { return name_; }

int Author::age(int currentYear) const {
    return currentYear - birthYear_;
}

void Author::setCountry(const std::string& country) {
    country_ = country;
}

bool Author::isContemporary(int currentYear) const {
    return age(currentYear) <= 100;
}


Publisher::Publisher(int i, const std::string& n, const Address& a)
    : id_(i), name_(n), address_(a), foundedYear_(2000) {}

std::string Publisher::getPublisher() const { return name_; }

void Publisher::setFoundedYear(int year) {
    if (year <= 0) throw WarehouseException("Некорректный год основания");
    foundedYear_ = year;
}

int Publisher::yearsOnMarket(int currentYear) const {
    return currentYear - foundedYear_;
}