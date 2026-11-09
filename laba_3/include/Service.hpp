#pragma once

#include <string>
#include <vector>
#include "except.hpp"
#include "Basic.hpp"
#include "People.hpp"

class TransferDriver;

class TravelService {
protected:
    int id_;
    std::string title_;
    int availableSlots_;
    double basePrice_ = 0.0;
public:
    TravelService(int i, const std::string& t, int slots);
    virtual ~TravelService();
    void setPrice(double price);
    double getBasePrice() const; 
    double totalRevenue() const;
    void releaseSlots(int n);
    void bookSlots(int n);
    bool isAvailable() const;
    virtual double cancellationFee(int daysBefore) const;
    virtual double durationDays() const = 0;
    virtual std::string describe() const;
};

class FlightBooking : public TravelService {
protected:
    int flightNumber_;
private:
    std::string flightCode_;
    const TravelAgent* agent_;
    const TourOperator* operator_;
    const TourCategory* category_;
    int year_ = 2026;
public:
    FlightBooking(int id,
                  const std::string& title,
                  int slots,
                  int flightNumber,
                  const std::string& flightCode,
                  const TravelAgent& agent,
                  const TourOperator& tourOp,
                  const TourCategory& category);

    double durationDays() const override;
    std::string describe() const override;

    bool isBookedVia(const TravelAgent& agent) const;
    bool isAllowedFor(int age) const;
    void setYear(int year);
    bool isUpcoming(int currentYear) const;
    std::string routeSummary() const; 
};


class EconomyFlight : public FlightBooking {
    double baggageFee_;
public:
    EconomyFlight(int i, const std::string& t, int s, int fn,
                  const std::string& code, const TravelAgent& a,
                  const TourOperator& op, const TourCategory& c, double bagFee);
    double durationDays() const override;
    std::string describe() const override;
    double totalBaggageCost(double kg) const; 
};


class BusinessFlight : public FlightBooking {
    bool vipLoungeAccess_;
public:
    BusinessFlight(int i, const std::string& t, int s, int fn,
                   const std::string& code, const TravelAgent& a,
                   const TourOperator& op, const TourCategory& c, bool vip);
    bool hasVipAccess() const;
    double loungeValue() const; 
    std::string describe() const override;
};


class CruiseBooking : public TravelService {
    int cabinCount_;
    bool balcony_;
public:
    CruiseBooking(int i, const std::string& t, int s, int cabins, bool balcony);
    double durationDays() const override;
    std::string describe() const override;
    double cabinRevenue() const;
    bool hasBalcony() const;
};


class HotelBooking : public TravelService {
    int nights_;
    int stars_;
public:
    HotelBooking(int i, const std::string& t, int slots, int nights, int stars);
    double durationDays() const override;
    std::string describe() const override;
    bool isLuxury() const;
    double nightlyRate() const;
};


class ExcursionBooking : public TravelService {
    int durationHours_;
    bool guideIncluded_;
public:
    ExcursionBooking(int i, const std::string& t, int slots, int hours, bool guide);
    double durationDays() const override;
    std::string describe() const override;
    double guideCost() const;
};


class Transfer : public TravelService {
    const TransferDriver* driver_;
    Location pickup_;
    Location dropoff_;
    double distanceKm_;
    bool completed_ = false;
public:
    Transfer(int i, const std::string& t, int slots, const TransferDriver* driver,
             const Location& pickup, const Location& dropoff, double distanceKm);
    double durationDays() const override;
    std::string describe() const override;
    double estimatedMinutes() const;
    void complete();
    bool isLongDistance() const;
};


class TravelPackage {
private:
    std::string name_;
    std::vector<TravelService*> services_;
    double discountPercent_ = 0.0;
public:
    TravelPackage(const std::string& name, double discountPercent);
    void addService(TravelService* service);
    double totalPrice() const;
    int totalDuration() const;
    bool isComplete() const;
    std::string itinerarySummary() const;
};


class InsuranceService : public TravelService {
    double coverageAmount_;
    std::string providerName_;
public:
    InsuranceService(int i, const std::string& t, int s, double coverage,
                     const std::string& provider);
    double durationDays() const override;
    std::string describe() const override;
    double premium() const;
    bool coversEvent(const std::string& event) const;
};