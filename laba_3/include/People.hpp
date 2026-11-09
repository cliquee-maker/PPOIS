#pragma once

#include <string>
#include <vector>
#include "except.hpp"
#include "Basic.hpp"
#include "Service.hpp"

class Transfer;

class Person {
protected:
    int id_;
    std::string name_;
    Location location_;
    ContactInfo contact_;
public:
    Person(int i, const std::string& n, const Location& loc);
    std::string getLocation() const;
    virtual ~Person();
    std::string getName() const;
    void setPhone(const std::string& phone);
    bool isValidPhone() const;
    virtual std::string role() const;
    bool livesIn(const std::string& country) const;
};

class Employee : virtual public Person {
protected:
    double salary_;
    int experienceYears_ = 0;
public:
    Employee(int i, const std::string& n, const Location& loc, double s);
    double getSalary() const;
    void raiseSalary(double percent);
    double annualSalary() const;
    bool isSenior() const;
    double monthlyBonus() const;
    std::string role() const override;
};

class Guide : virtual public Employee {
    std::vector<std::string> languages_;
    std::string licenseNumber_;
public:
    Guide(int i, const std::string& n, const Location& loc, double s,
          const std::string& license);
    bool speaks(const std::string& language) const;
    void addLanguage(const std::string& language);
    bool isLicensed() const;
    std::string role() const override;
};

class SupportAgent :virtual public Employee {
    std::string shiftStart_;
    int resolvedTickets_ = 0;
public:
    SupportAgent(int i, const std::string& n, const Location& loc, double s,
                 const std::string& shiftStart);
    void resolveTicket();
    bool isOnShift(const std::string& time) const;
    std::string role() const override;
};

class Tourist : virtual public Person {
    int bonusMiles_;
    double totalSpent_ = 0.0;
    std::string passportNumber_;
public:
    Tourist(int i, const std::string& n, const Location& loc);
    void addMiles(int m);
    void spendMiles(int m);
    void registerBooking(double sum);
    void setPassport(const std::string& passport);
    bool canRedeem(int miles) const;
    std::string loyaltyTier() const;
    std::string role() const override;
};

class HotelPartner : public Person {
    std::string hotelName_;
    int rating_ = 5;
public:
    HotelPartner(int i, const std::string& n, const Location& loc, const std::string& hotel);
    void rate(int stars);
    bool isVerified() const;
    double commissionRate() const;
    std::string role() const override;
};

class TourManager : virtual public Employee {
    int managedAgents_;
public:
    TourManager(int i, const std::string& n, const Location& loc, double s, int agents);
    int getManagedAgentsCount() const;
    void assignAgent(Employee& e);
    double bonus() const;
    bool isOverloaded() const;
    std::string role() const override;
};

class TransferDriver : virtual public Employee {
    std::string vehicleModel_;
    double distanceDrivenKm_ = 0.0;
public:
    TransferDriver(int i, const std::string& n, const Location& loc, double s,
                   const std::string& vehicle);
    bool canDrive() const;
    void executeTransfer(Transfer& t);
    void addDistance(double km);
    double fuelCost(double pricePerLiter) const;
    std::string role() const override;
};