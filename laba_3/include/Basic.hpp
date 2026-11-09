#pragma once

#include <string>
#include "except.hpp"

class Currency {
private:
    std::string code_;
    std::string symbol_;
    double rateToUsd_ = 1.0;
public:
    Currency(const std::string& code, const std::string& symbol, double rateToUsd);
    double convertFromUsd(double usd) const;
    double convertToUsd(double amount) const;
    std::string format(double amount) const;
    bool isSame(const Currency& other) const;
};

class Price {
private:
    double price_;
    std::string currency_;
public:
    Price(double p, const std::string& currency);
    double getPriceWithTax(int taxPercent = 20) const;
    double getPriceWithDiscount(int salePercent) const;
    Price convertTo(double rate, const std::string& currency) const;
    Price add(const Price& other) const;
    Price subtract(const Price& other) const;
    bool isAffordable(double budget) const;
};

class DateRange {
private:
    std::string startDate_;
    std::string endDate_;
public:
    DateRange(const std::string& start, const std::string& end);
    int nights() const;
    bool overlaps(const DateRange& other) const;
    bool contains(const std::string& date) const;
    void extend(int days);
};

class ContactInfo {
private:
    std::string phone_;
    std::string email_;
    std::string emergencyPhone_;
public:
    ContactInfo() = default;
    ContactInfo(const std::string& phone, const std::string& email);
    bool isValidEmail() const;
    bool isValidPhone() const;
    void setEmergencyContact(const std::string& phone);
    std::string maskedPhone() const;
};

class Location {
private:
    std::string country_;
    std::string city_;
    std::string airportCode_;
public:
    Location(const std::string& country, const std::string& city, const std::string& airportCode);
    std::string getFullLocation() const;
    std::string getCountry() const;
    std::string getAirportCode() const;
    bool sameCountry(const Location& other) const;
    double distanceTo(const Location& other) const;
};

// Норма багажа для перелёта или круиза.
class BaggageAllowance {
private:
    double maxWeightKg_;
    int maxPieces_;
    double maxVolumeM3_;
public:
    BaggageAllowance(double weight, int pieces, double volume);
    double getMaxWeight() const;
    int getMaxPieces() const;
    bool fits(double weight, double volume) const;
    double excessWeight(double weight) const;
    bool requiresExtraFee(double weight) const;
};

class TourCategory {
private:
    int id_;
    std::string name_;
    int ageLimit_;
public:
    TourCategory(int id, const std::string& name, int ageLimit);
    std::string getCategoryName() const;
    int getAgeLimit() const;
    bool allowedFor(int age) const;
    bool matches(const TourCategory& other) const;
    std::string describe() const;
};

class TravelAgent {
private:
    int id_;
    std::string name_;
    int experienceYears_;
    std::string agencyName_ = "Global Travel";
public:
    TravelAgent(int i, const std::string& n, int exp);
    std::string getAgentInfo() const;
    int experienceYears() const;
    void setAgencyName(const std::string& agency);
    bool isTopSeller(int threshold) const;
    bool canHandle(const TourCategory& category) const;
};

class TourOperator {
private:
    int id_;
    std::string name_;
    Location hqLocation_;
    int foundedYear_;
public:
    TourOperator(int i, const std::string& n, const Location& loc);
    std::string getOperatorInfo() const;
    void setFoundedYear(int year);
    int yearsOnMarket(int currentYear) const;
    bool isVeteran(int threshold) const;
    bool serves(const Location& destination) const;
};

class Discount {
private:
    std::string code_;
    double percent_;
    DateRange validPeriod_;
public:
    Discount(const std::string& code, double percent, const DateRange& period);
    bool isActive(const std::string& date) const;
    double apply(double amount) const;
    bool isBetterThan(const Discount& other) const;
    std::string describe() const;
};
