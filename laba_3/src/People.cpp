#include "People.hpp"
#include <algorithm>

Person::Person(int i, const std::string& n, const Location& loc)
    : id_(i), name_(n), location_(loc), contact_() {
    if (name_.empty()) {
        throw TravelException("Имя человека не может быть пустым.");
    }
}

std::string Person::getLocation() const {
    return location_.getFullLocation();
}

Person::~Person() = default;

std::string Person::getName() const {
    return name_;
}

void Person::setPhone(const std::string& phone) {
    contact_.setPhone(phone);
}

bool Person::isValidPhone() const {
    return contact_.isValidPhone();
}

std::string Person::role() const {
    return "Person";
}

bool Person::livesIn(const std::string& country) const {
    return location_.getCountry() == country;
}


Employee::Employee(int i, const std::string& n, const Location& loc, double s)
    : Person(i, n, loc), salary_(s) {
    if (salary_ < 0.0) {
        throw InvalidPriceException("Зарплата не может быть отрицательной.");
    }
}

double Employee::getSalary() const {
    return salary_;
}

void Employee::raiseSalary(double percent) {
    if (percent < 0.0) {
        throw InvalidPriceException("Процент повышения не может быть отрицательным.");
    }
    salary_ *= (1.0 + percent / 100.0);
}

double Employee::annualSalary() const {
    return salary_ * 12.0;
}

bool Employee::isSenior() const {
    return experienceYears_ >= 5;
}

double Employee::monthlyBonus() const {
    return salary_ * 0.1 + experienceYears_ * 50.0;
}

std::string Employee::role() const {
    return "Employee";
}


Guide::Guide(int i, const std::string& n, const Location& loc, double s,
             const std::string& license)
    : Person(i, n, loc), Employee(i, n, loc, s), 
      languages_{"English"}, licenseNumber_(license) {}

bool Guide::speaks(const std::string& language) const {
    return std::find(languages_.begin(), languages_.end(), language) != languages_.end();
}

void Guide::addLanguage(const std::string& language) {
    if (language.empty()) {
        throw TravelException("Название языка не может быть пустым.");
    }
    if (!speaks(language)) {
        languages_.push_back(language);
    }
}

bool Guide::isLicensed() const {
    return !licenseNumber_.empty();
}

std::string Guide::role() const {
    return "Guide";
}


Tourist::Tourist(int i, const std::string& n, const Location& loc)
    : Person(i, n, loc), bonusMiles_(0), totalSpent_(0.0) {}

void Tourist::addMiles(int m) {
    if (m < 0) {
        throw ReservationException("Количество бонусных миль не может быть отрицательным.");
    }
    bonusMiles_ += m;
}

void Tourist::spendMiles(int m) {
    if (m <= 0 || m > bonusMiles_) {
        throw ReservationException("Недостаточно бонусных миль.");
    }
    bonusMiles_ -= m;
}

void Tourist::registerBooking(double sum) {
    if (sum < 0.0) {
        throw InvalidPriceException("Сумма бронирования не может быть отрицательной.");
    }
    totalSpent_ += sum;
    addMiles(static_cast<int>(sum / 100.0));
}

void Tourist::setPassport(const std::string& passport) {
    if (passport.size() < 5) {
        throw InvalidPassportException("Некорректный номер паспорта.");
    }
    passportNumber_ = passport;
}

bool Tourist::canRedeem(int miles) const {
    return miles > 0 && miles <= bonusMiles_;
}

std::string Tourist::loyaltyTier() const {
    if (totalSpent_ >= 10000.0) {
        return "Platinum";
    }
    if (totalSpent_ >= 5000.0) {
        return "Gold";
    }
    if (totalSpent_ >= 1000.0) {
        return "Silver";
    }
    return "Bronze";
}

std::string Tourist::role() const {
    return "Tourist";
}


HotelPartner::HotelPartner(int i, const std::string& n, const Location& loc,
                           const std::string& hotel)
    : Person(i, n, loc), hotelName_(hotel), rating_(5) {
    if (hotelName_.empty()) {
        throw TravelException("Название отеля не может быть пустым.");
    }
}

void HotelPartner::rate(int stars) {
    if (stars < 1 || stars > 5) {
        throw TravelException("Рейтинг отеля должен быть от 1 до 5.");
    }
    rating_ = stars;
}

bool HotelPartner::isVerified() const {
    return rating_ >= 4;
}

double HotelPartner::commissionRate() const {
    return 0.08 + rating_ * 0.01;
}

std::string HotelPartner::role() const {
    return "HotelPartner";
}


TransferDriver::TransferDriver(int i, const std::string& n, const Location& loc,
                               double s, const std::string& vehicle)
    : Person(i, n, loc), Employee(i, n, loc, s), 
      vehicleModel_(vehicle), distanceDrivenKm_(0.0) {
    if (vehicleModel_.empty()) {
        throw TravelException("Модель автомобиля не может быть пустой.");
    }
}

bool TransferDriver::canDrive() const {
    return !vehicleModel_.empty();
}

void TransferDriver::executeTransfer(Transfer& t) {
    t.complete();
}

void TransferDriver::addDistance(double km) {
    if (km < 0.0) {
        throw InvalidPriceException("Пробег не может быть отрицательным.");
    }
    distanceDrivenKm_ += km;
}

double TransferDriver::fuelCost(double pricePerLiter) const {
    if (pricePerLiter < 0.0) {
        throw InvalidPriceException("Цена топлива не может быть отрицательной.");
    }
    return distanceDrivenKm_ * 0.08 * pricePerLiter;
}

std::string TransferDriver::role() const {
    return "TransferDriver";
}


SupportAgent::SupportAgent(int i, const std::string& n, const Location& loc, double s,
                           const std::string& shiftStart)
    : Person(i, n, loc), Employee(i, n, loc, s), 
      shiftStart_(shiftStart), resolvedTickets_(0) {
    if (shiftStart_.empty()) {
        throw TravelException("Время начала смены не может быть пустым.");
    }
}

void SupportAgent::resolveTicket() {
    ++resolvedTickets_;
}

bool SupportAgent::isOnShift(const std::string& time) const {
    return time >= shiftStart_;
}

std::string SupportAgent::role() const {
    return "SupportAgent";
}


TourManager::TourManager(int i, const std::string& n, const Location& loc, double s,
                         int agents)
    : Person(i, n, loc), Employee(i, n, loc, s), 
      managedAgents_(agents){
    if (managedAgents_ < 0) {
        throw TravelException("Количество подчинённых агентов не может быть отрицательным.");
    }
}

int TourManager::getManagedAgentsCount() const {
    return managedAgents_;
}

void TourManager::assignAgent(Employee& e) {
    if (static_cast<Employee*>(this) == &e) {
        throw TravelException("Нельзя назначить менеджера своим же агентом.");
    }
    ++managedAgents_;
}

double TourManager::bonus() const {
    return salary_ * 0.2 + managedAgents_ * 500.0;
}

bool TourManager::isOverloaded() const {
    return managedAgents_ > 10;
}

std::string TourManager::role() const {
    return "TourManager";
}