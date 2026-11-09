#include "Booking.hpp"
#include <cmath>
#include <sstream>


Reservation::Reservation(int id, const Tourist* tourist, const DateRange& period,
                         const Price& total)
    : id_(id), tourist_(tourist), services_(), period_(period),
      status_("pending"), total_(total) {
    if (id_ < 0) {
        throw ReservationException("Идентификатор бронирования не может быть отрицательным.");
    }
}

bool Reservation::confirm() {
    if (services_.empty()) {
        throw ReservationException("Нельзя подтвердить бронирование без услуг.");
    }
    status_ = "confirmed";
    return true;
}

void Reservation::cancel() {
    status_ = "cancelled";
}

double Reservation::total() const {
    return total_.amount();
}

bool Reservation::overlaps(const Reservation& other) const {
    return period_.overlaps(other.period_);
}

void Reservation::addService(TravelService* service) {
    if (service == nullptr) {
        throw ReservationException("Нельзя добавить пустую услугу в бронирование.");
    }
    services_.push_back(service);
}

std::string Reservation::summary() const {
    const std::string who = tourist_ != nullptr ? tourist_->getName() : "аноним";
    std::ostringstream oss;
    oss << "Бронь #" << id_ << " (" << who << "): "
        << services_.size() << " услуг, "
        << period_.getStart() << " — " << period_.getEnd()
        << ", статус: " << status_
        << ", сумма: " << total_.amount();
    return oss.str();
}

int Reservation::nights() const {
    return period_.nights();
}


BookingRequest::BookingRequest(const Tourist* tourist, const DateRange& dates,
                               int partySize)
    : tourist_(tourist), desired_(), dates_(dates), partySize_(partySize) {
    if (partySize_ <= 0) {
        throw ReservationException("Размер группы должен быть положительным.");
    }
}

void BookingRequest::addDesiredService(const TravelService* service) {
    if (service == nullptr) {
        throw ReservationException("Нельзя добавить пустую услугу в запрос.");
    }
    desired_.push_back(service);
}

bool BookingRequest::canBeSatisfied() const {
    return !desired_.empty() && partySize_ > 0 && dates_.nights() > 0;
}

std::string BookingRequest::describe() const {
    const std::string who = tourist_ != nullptr ? tourist_->getName() : "аноним";
    std::ostringstream oss;
    oss << "Запрос от " << who << " на " << partySize_ << " чел., "
        << dates_.getStart() << " — " << dates_.getEnd()
        << ", услуг: " << desired_.size();
    return oss.str();
}


void Itinerary::addReservation(Reservation* reservation) {
    if (reservation == nullptr) {
        throw ReservationException("Нельзя добавить пустое бронирование в маршрут.");
    }
    reservations_.push_back(reservation);
}

int Itinerary::totalDays() const {
    int days = 0;
    for (const Reservation* reservation : reservations_) {
        if (reservation != nullptr) {
            days += reservation->nights();
        }
    }
    return days;
}

double Itinerary::totalCost() const {
    double sum = 0.0;
    for (const Reservation* reservation : reservations_) {
        if (reservation != nullptr) {
            sum += reservation->total();
        }
    }
    return sum;
}

std::string Itinerary::timeline() const {
    std::ostringstream oss;
    for (std::size_t i = 0; i < reservations_.size(); ++i) {
        if (reservations_[i] != nullptr) {
            oss << (i + 1) << ". " << reservations_[i]->summary() << "\n";
        }
    }
    return oss.str();
}


SearchQuery::SearchQuery(const Location& from, const Location& to,
                         const DateRange& dates, int guests, double maxBudget)
    : from_(from), to_(to), dates_(dates), guests_(guests), maxBudget_(maxBudget) {
    if (guests_ <= 0) {
        throw ReservationException("Количество гостей должно быть положительным.");
    }
    if (maxBudget_ < 0.0) {
        throw InvalidPriceException("Бюджет не может быть отрицательным.");
    }
}

bool SearchQuery::matches(const TravelService& service) const {
    return service.durationDays() <= static_cast<double>(dates_.nights());
}

std::string SearchQuery::cacheKey() const {
    std::ostringstream oss;
    oss << from_.getAirportCode() << '-' << to_.getAirportCode()
        << '-' << dates_.getStart() << '-' << guests_;
    return oss.str();
}

void SearchQuery::adjustBudget(double delta) {
    maxBudget_ += delta;
    if (maxBudget_ < 0.0) {
        throw InvalidPriceException("Бюджет не может быть отрицательным.");
    }
}


Availability::Availability(const TravelService* service, const DateRange& period,
                           int freeSlots)
    : service_(service), period_(period), freeSlots_(freeSlots) {
    if (service_ == nullptr) {
        throw ReservationException("Доступность должна ссылаться на услугу.");
    }
    if (freeSlots_ < 0) {
        throw ReservationException("Количество свободных мест не может быть отрицательным.");
    }
}

bool Availability::isAvailable(int people) const {
    return people > 0 && people <= freeSlots_;
}

void Availability::hold(int people) {
    if (people <= 0) {
        throw ReservationException("Количество мест должно быть положительным.");
    }
    if (people > freeSlots_) {
        throw FullyBookedException("Недостаточно свободных мест.");
    }
    freeSlots_ -= people;
}

void Availability::release(int people) {
    if (people <= 0) {
        throw ReservationException("Количество возвращаемых мест должно быть положительным.");
    }
    freeSlots_ += people;
}

