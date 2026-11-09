#include "Service.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>


TravelService::TravelService(int i, const std::string& t, int slots)
    : id_(i), title_(t), availableSlots_(slots), basePrice_(0.0) {
    if (id_ < 0) {
        throw TravelException("Идентификатор услуги не может быть отрицательным.");
    }
    if (title_.empty()) {
        throw TravelException("Название услуги не может быть пустым.");
    }
    if (availableSlots_ < 0) {
        throw ReservationException("Количество мест не может быть отрицательным.");
    }
}

TravelService::~TravelService() = default;

void TravelService::setPrice(double price) {
    if (price < 0.0) {
        throw InvalidPriceException("Цена услуги не может быть отрицательной.");
    }
    basePrice_ = price;
}

double TravelService::getBasePrice() const {
    return basePrice_;
}

double TravelService::totalRevenue() const {
    return basePrice_ * static_cast<double>(availableSlots_);
}

void TravelService::releaseSlots(int n) {
    if (n < 0) {
        throw ReservationException("Количество возвращаемых мест не может быть отрицательным.");
    }
    availableSlots_ += n;
}

void TravelService::bookSlots(int n) {
    if (n <= 0) {
        throw ReservationException("Количество бронируемых мест должно быть положительным.");
    }
    if (n > availableSlots_) {
        throw FullyBookedException("Недостаточно свободных мест.");
    }
    availableSlots_ -= n;
}

bool TravelService::isAvailable() const {
    return availableSlots_ > 0;
}

double TravelService::cancellationFee(int daysBefore) const {
    if (daysBefore >= 14) {
        return 0.0;
    }
    if (daysBefore >= 7) {
        return basePrice_ * 0.25;
    }
    if (daysBefore >= 2) {
        return basePrice_ * 0.50;
    }
    return basePrice_ * 0.80;
}

std::string TravelService::describe() const {
    std::ostringstream oss;
    oss << title_ << " (свободно мест: " << availableSlots_ << ")";
    return oss.str();
}


FlightBooking::FlightBooking(int id,
                             const std::string& title,
                             int slots,
                             int flightNumber,
                             const std::string& flightCode,
                             const TravelAgent& agent,
                             const TourOperator& tourOp,
                             const TourCategory& category)
    : TravelService(id, title, slots),
      flightNumber_(flightNumber),
      flightCode_(flightCode),
      agent_(&agent),
      operator_(&tourOp),
      category_(&category),
      year_(2026) {
    if (flightNumber_ <= 0) {
        throw ReservationException("Номер рейса должен быть положительным.");
    }
    if (flightCode_.empty()) {
        throw ReservationException("Код рейса не может быть пустым.");
    }
}

double FlightBooking::durationDays() const {
    return 1.0;
}

std::string FlightBooking::describe() const {
    std::ostringstream oss;
    oss << "Перелёт " << flightCode_ << " (рейс " << flightNumber_ << ")";
    if (operator_ != nullptr) {
        oss << ", " << operator_->getOperatorInfo();
    }
    if (category_ != nullptr) {
        oss << ", " << category_->describe();
    }
    return oss.str();
}

bool FlightBooking::isBookedVia(const TravelAgent& agent) const {
    return agent_ == &agent;
}

bool FlightBooking::isAllowedFor(int age) const {
    return category_ == nullptr || category_->allowedFor(age);
}

void FlightBooking::setYear(int year) {
    if (year < 1900 || year > 2100) {
        throw InvalidDateRangeException("Некорректный год перелёта.");
    }
    year_ = year;
}

bool FlightBooking::isUpcoming(int currentYear) const {
    return year_ >= currentYear;
}

std::string FlightBooking::routeSummary() const {
    std::ostringstream oss;
    oss << flightCode_ << " (" << flightNumber_ << ")";
    if (operator_ != nullptr) {
        oss << " — " << operator_->getOperatorInfo();
    }
    return oss.str();
}


EconomyFlight::EconomyFlight(int i, const std::string& t, int s, int fn,
                             const std::string& code, const TravelAgent& a,
                             const TourOperator& op, const TourCategory& c,
                             double bagFee)
    : FlightBooking(i, t, s, fn, code, a, op, c), baggageFee_(bagFee) {
    if (baggageFee_ < 0.0) {
        throw InvalidPriceException("Плата за багаж не может быть отрицательной.");
    }
}

double EconomyFlight::durationDays() const {
    return FlightBooking::durationDays();
}

std::string EconomyFlight::describe() const {
    return FlightBooking::describe() + ", эконом-класс";
}

double EconomyFlight::totalBaggageCost(double kg) const {
    if (kg < 0.0) {
        throw ExcessBaggageException("Вес багажа не может быть отрицательным.");
    }
    return kg * baggageFee_;
}


BusinessFlight::BusinessFlight(int i, const std::string& t, int s, int fn,
                               const std::string& code, const TravelAgent& a,
                               const TourOperator& op, const TourCategory& c,
                               bool vip)
    : FlightBooking(i, t, s, fn, code, a, op, c), vipLoungeAccess_(vip) {}

bool BusinessFlight::hasVipAccess() const {
    return vipLoungeAccess_;
}

double BusinessFlight::loungeValue() const {
    return vipLoungeAccess_ ? 150.0 : 0.0;
}

std::string BusinessFlight::describe() const {
    return FlightBooking::describe() + ", бизнес-класс"
        + (vipLoungeAccess_ ? " (VIP-зал)" : "");
}


CruiseBooking::CruiseBooking(int i, const std::string& t, int s, int cabins, bool balcony)
    : TravelService(i, t, s), cabinCount_(cabins), balcony_(balcony) {
    if (cabinCount_ <= 0) {
        throw ReservationException("Количество кают должно быть положительным.");
    }
}

double CruiseBooking::durationDays() const {
    return 7.0;
}

std::string CruiseBooking::describe() const {
    std::ostringstream oss;
    oss << "Круиз " << title_ << ", кают: " << cabinCount_
        << (balcony_ ? ", с балконом" : ", без балкона");
    return oss.str();
}

double CruiseBooking::cabinRevenue() const {
    return cabinCount_ * getBasePrice() * (balcony_ ? 1.3 : 1.0);
}

bool CruiseBooking::hasBalcony() const {
    return balcony_;
}


HotelBooking::HotelBooking(int i, const std::string& t, int slots, int nights, int stars)
    : TravelService(i, t, slots), nights_(nights), stars_(stars) {
    if (nights_ <= 0) {
        throw ReservationException("Количество ночей должно быть положительным.");
    }
    if (stars_ < 1 || stars_ > 5) {
        throw TravelException("Количество звёзд отеля должно быть от 1 до 5.");
    }
}

double HotelBooking::durationDays() const {
    return static_cast<double>(nights_);
}

std::string HotelBooking::describe() const {
    std::ostringstream oss;
    oss << "Отель " << title_ << ", " << nights_ << " ноч., " << stars_ << "★";
    return oss.str();
}

bool HotelBooking::isLuxury() const {
    return stars_ >= 5;
}

double HotelBooking::nightlyRate() const {
    return getBasePrice() / static_cast<double>(nights_);
}


ExcursionBooking::ExcursionBooking(int i, const std::string& t, int slots, int hours,
                                   bool guide)
    : TravelService(i, t, slots), durationHours_(hours), guideIncluded_(guide) {
    if (durationHours_ <= 0) {
        throw ReservationException("Длительность экскурсии должна быть положительной.");
    }
}

double ExcursionBooking::durationDays() const {
    return static_cast<double>(durationHours_) / 24.0;
}

std::string ExcursionBooking::describe() const {
    std::ostringstream oss;
    oss << "Экскурсия " << title_ << ", " << durationHours_ << " ч."
        << (guideIncluded_ ? ", с гидом" : ", без гида");
    return oss.str();
}

double ExcursionBooking::guideCost() const {
    return guideIncluded_ ? 50.0 : 0.0;
}


Transfer::Transfer(int i, const std::string& t, int slots, const TransferDriver* driver,
                   const Location& pickup, const Location& dropoff, double distanceKm)
    : TravelService(i, t, slots), driver_(driver), pickup_(pickup),
      dropoff_(dropoff), distanceKm_(distanceKm), completed_(false) {
    if (distanceKm_ < 0.0) {
        throw InvalidPriceException("Расстояние трансфера не может быть отрицательным.");
    }
}

double Transfer::durationDays() const {
    return estimatedMinutes() / (24.0 * 60.0);
}

std::string Transfer::describe() const {
    std::ostringstream oss;
    oss << "Трансфер " << pickup_.getFullLocation() << " → "
        << dropoff_.getFullLocation();
    if (driver_ != nullptr) {
        oss << ", водитель: " << driver_->getName();
    }
    oss << (completed_ ? " (выполнен)" : " (ожидается)");
    return oss.str();
}

double Transfer::estimatedMinutes() const {
    return distanceKm_ * 1.5;
}

void Transfer::complete() {
    if (completed_) {
        throw ReservationException("Трансфер уже выполнен.");
    }
    completed_ = true;
}

bool Transfer::isLongDistance() const {
    return distanceKm_ > 50.0;
}


TravelPackage::TravelPackage(const std::string& name, double discountPercent)
    : name_(name), discountPercent_(discountPercent) {
    if (name_.empty()) {
        throw TravelException("Название пакета не может быть пустым.");
    }
    if (discountPercent_ < 0.0 || discountPercent_ > 100.0) {
        throw InvalidPromoCodeException("Некорректный процент скидки пакета.");
    }
}

void TravelPackage::addService(TravelService* service) {
    if (service == nullptr) {
        throw ReservationException("Нельзя добавить пустую услугу в пакет.");
    }
    services_.push_back(service);
}

double TravelPackage::totalPrice() const {
    double sum = 0.0;
    for (const TravelService* service : services_) {
        if (service != nullptr) {
            sum += service->getBasePrice();
        }
    }
    return sum * (1.0 - discountPercent_ / 100.0);
}

int TravelPackage::totalDuration() const {
    double sum = 0.0;
    for (const TravelService* service : services_) {
        if (service != nullptr) {
            sum += service->durationDays();
        }
    }
    return static_cast<int>(std::ceil(sum));
}

bool TravelPackage::isComplete() const {
    if (services_.empty()) {
        return false;
    }
    return std::all_of(services_.begin(), services_.end(),
                       [](const TravelService* service) { return service != nullptr; });
}

std::string TravelPackage::itinerarySummary() const {
    std::ostringstream oss;
    oss << "Пакет \"" << name_ << "\" (" << services_.size() << " услуг):\n";
    for (const TravelService* service : services_) {
        if (service != nullptr) {
            oss << "  - " << service->describe() << "\n";
        }
    }
    return oss.str();
}


InsuranceService::InsuranceService(int i, const std::string& t, int s, double coverage,
                                   const std::string& provider)
    : TravelService(i, t, s), coverageAmount_(coverage), providerName_(provider) {
    if (coverageAmount_ < 0.0) {
        throw InvalidPriceException("Сумма покрытия не может быть отрицательной.");
    }
    if (providerName_.empty()) {
        throw TravelException("Название страховщика не может быть пустым.");
    }
}

double InsuranceService::durationDays() const {
    return 0.0;
}

std::string InsuranceService::describe() const {
    std::ostringstream oss;
    oss << "Страхование от " << providerName_ << ", покрытие "
        << std::fixed << std::setprecision(2) << coverageAmount_;
    return oss.str();
}

double InsuranceService::premium() const {
    return coverageAmount_ * 0.05;
}

bool InsuranceService::coversEvent(const std::string& event) const {
    return event == "medical" || event == "accident"
        || event == "baggage" || event == "trip";
}
