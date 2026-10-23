#pragma once

#include <string>
#include "except.hpp"
#include "Basic.hpp"

class Pallet;
class Shipment;

class Person {
protected:
    int id_;
    std::string name_;
    Address address_;
    std::string phone_;
public:
    Person(int i, const std::string& n, const Address& a);
    std::string getAddress() const;
    virtual ~Person();
    std::string getName() const;
    void setPhone(const std::string& phone);
    bool isValidPhone() const;
    virtual std::string role() const;
};

class Employee : virtual public Person {
    double salary_;
    int experienceYears_ = 0;
public:
    Employee(int i, const std::string& n, const Address& a, double s);
    double getSalary() const;
    void raiseSalary(double percent);
    double annualSalary() const;
    std::string role() const override;
};

class Customer : virtual public Person {
    int bonusPoints_;
    double totalSpent_ = 0.0;
public:
    Customer(int i, const std::string& n, const Address& a);
    void addPoints(int p);
    void spendPoints(int p);
    void registerPurchase(double sum);
    std::string role() const override;
};

class Supplier : virtual public Person {
    std::string company_;
    int rating_ = 5;
public:
    Supplier(int i, const std::string& n, const Address& a, const std::string& c);
    void rate(int stars);
    bool isReliable() const;
    std::string role() const override;
};

class Manager : public Employee {
    int teamSize_;
public:
    Manager(int i, const std::string& n, const Address& a, double s, int t);
    int getTeamSize() const;
    void addToTeam(Employee& e);
    double bonus() const;
    std::string role() const override;
};

class Loader : public Employee {
    double maxLift_;
    int shiftHours_ = 8;
public:
    Loader(int i, const std::string& n, const Address& a, double s, double m);
    bool canLift(double kg) const;
    bool canLiftPallet(const Pallet& p) const;
    bool canWorkOvertime() const;
    std::string role() const override;
};

class Driver : public Employee {
    std::string license_;
    double kmDriven_ = 0.0;
public:
    Driver(int i, const std::string& n, const Address& a, double s, const std::string& l);
    bool hasLicense() const;
    void deliver(Shipment& s);
    void addDistance(double km);
    std::string role() const override;
};