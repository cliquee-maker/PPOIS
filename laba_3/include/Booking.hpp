#pragma once

#include <string>
#include <vector>
#include "except.hpp"
#include "Basic.hpp"

class Tourist;
class TravelService;

class Reservation {
private:
    int id_;
    const Tourist* tourist_;
    std::vector<TravelService*> services_;
    DateRange period_;
    std::string status_;
    Price total_;
public:
    Reservation(int id, const Tourist* tourist, const DateRange& period,
                const Price& total);
    bool confirm();
    void cancel();
    double total() const;
    bool overlaps(const Reservation& other) const;
    void addService(TravelService* service);
    std::string summary() const;
};

class BookingRequest {
private:
    const Tourist* tourist_;
    std::vector<const TravelService*> desired_;
    DateRange dates_;
    int partySize_;
public:
    BookingRequest(const Tourist* tourist, const DateRange& dates, int partySize);
    void addDesiredService(const TravelService* service);
    bool canBeSatisfied() const;
    std::string describe() const;
};

class Itinerary {
private:
    std::vector<Reservation*> reservations_;
public:
    Itinerary() = default;
    void addReservation(Reservation* reservation);
    int totalDays() const;
    double totalCost() const;
    std::string timeline() const;
};

class SearchQuery {
private:
    Location from_;
    Location to_;
    DateRange dates_;
    int guests_;
    double maxBudget_;
public:
    SearchQuery(const Location& from, const Location& to,
                const DateRange& dates, int guests, double maxBudget);
    bool matches(const TravelService& service) const;
    std::string cacheKey() const;
    void adjustBudget(double delta);
};

class Availability {
private:
    const TravelService* service_;
    DateRange period_;
    int freeSlots_;
public:
    Availability(const TravelService* service, const DateRange& period, int freeSlots);
    bool isAvailable(int people) const;
    void hold(int people);
    void release(int people);
};

class Seat {
private:
    std::string number_;
    std::string row_;
    bool window_;
    bool occupied_ = false;
public:
    Seat(const std::string& number, const std::string& row, bool window);
    bool isAvailable() const;
    void occupy();
    double extraFee() const;
    std::string label() const;
};
