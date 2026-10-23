#include "People.hpp"
#include "Storage.hpp"
#include "Operation.hpp"
#include <algorithm>
#include <cctype>


Person::Person(int i, const std::string& n, const Address& a)
    : id_(i), name_(n), address_(a) {}

Person::~Person() {}

std::string Person::getAddress() const {
    return address_.getAdress();
}

std::string Person::getName() const {
    return name_;
}

void Person::setPhone(const std::string& phone) {
    phone_ = phone;
}

bool Person::isValidPhone() const {
    if (phone_.size() < 11 || phone_[0] != '+') return false;
    return std::all_of(phone_.begin() + 1, phone_.end(),
                       [](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
}

std::string Person::role() const {
    return "person";
}


Employee::Employee(int i, const std::string& n, const Address& a, double s)
    : Person(i, n, a), salary_(s)
{
    if (s < 0) throw SalaryException("Employee salary cannot be negative.");
}

double Employee::getSalary() const {
    return salary_;
}

void Employee::raiseSalary(double percent) {
    if (percent <= 0 || percent > 100) throw SalaryException("Неверный процент повышения");
    salary_ *= (1.0 + percent / 100.0);
}

double Employee::annualSalary() const {
    return salary_ * 12 * (1.0 + 0.01 * experienceYears_); 
}

std::string Employee::role() const {
    return "employee";
}


Customer::Customer(int i, const std::string& n, const Address& a)
    : Person(i, n, a), bonusPoints_(0) {}

void Customer::addPoints(int p) {
    if (p <= 0) throw InvalidQuantityException("Баллы должны быть положительными");
    bonusPoints_ += p;
}

void Customer::spendPoints(int p) {
    if (p <= 0 || p > bonusPoints_) throw WarehouseException("Недостаточно бонусных баллов");
    bonusPoints_ -= p;
}

void Customer::registerPurchase(double sum) {
    if (sum <= 0) throw InvalidPriceException("Сумма покупки должна быть больше нуля");
    totalSpent_ += sum;
    bonusPoints_ += static_cast<int>(sum / 10);  
}

std::string Customer::role() const {
    return "customer";
}


Supplier::Supplier(int i, const std::string& n, const Address& a, const std::string& c)
    : Person(i, n, a), company_(c) {}

void Supplier::rate(int stars) {
    if (stars < 1 || stars > 5) throw InvalidRatingException("Оценка должна быть от 1 до 5");
    rating_ = (rating_ + stars) / 2;
}

bool Supplier::isReliable() const {
    return rating_ > 4;
}

std::string Supplier::role() const {
    return "supplier of " + company_;
}


Manager::Manager(int i, const std::string& n, const Address& a, double s, int t)
    : Person(i, n, a), Employee(i, n, a, s), teamSize_(t)
{
    if (t < 0) throw InvalidQuantityException("Размер команды не может быть отрицательным");
}

int Manager::getTeamSize() const {
    return teamSize_;
}

void Manager::addToTeam(Employee& e) {
    if (&e == this) throw WarehouseException("Менеджер не может добавить сам себя");
    ++teamSize_;
}

double Manager::bonus() const {
    return getSalary() * 0.05 * teamSize_;
}

std::string Manager::role() const {
    return "manager";
}


Loader::Loader(int i, const std::string& n, const Address& a, double s, double m)
    : Person(i, n, a), Employee(i, n, a, s), maxLift_(m) {}

bool Loader::canLift(double kg) const {
    return kg <= maxLift_;
}

bool Loader::canLiftPallet(const Pallet& p) const {
    return canLift(p.getLoad());
}

bool Loader::canWorkOvertime() const {
    return shiftHours_ < 12;
}

std::string Loader::role() const {
    return "loader";
}

Driver::Driver(int i, const std::string& n, const Address& a, double s, const std::string& l)
    : Person(i, n, a), Employee(i, n, a, s), license_(l) {}

bool Driver::hasLicense() const {
    return !license_.empty();
}

void Driver::deliver(Shipment& s) {
    if (!hasLicense()) throw WarehouseException("У водителя нет прав");
    s.sendShipment();
}

void Driver::addDistance(double km) {
    if (km <= 0) throw InvalidQuantityException("Расстояние должно быть больше нуля");
    kmDriven_ += km;
}

std::string Driver::role() const {
    return "driver";
}