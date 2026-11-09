#pragma once

#include <string>
#include "except.hpp"
#include "Basic.hpp"
#include "People.hpp" 

class Tourist;  

class TravelDocument {
protected:
    int documentNumber_;
    std::string issueDate_;
    bool verified_ = false;
public:
    TravelDocument(int n, const std::string& d);
    virtual ~TravelDocument();
    int getNumber() const;
    void verify();
    bool isVerified() const;
    virtual bool isValidOn(const std::string& date) const;
    virtual std::string title() const;
};

class Invoice : public TravelDocument {
    double amount_;
    double taxRate_ = 0.1;
    bool paid_ = false;
public:
    Invoice(int n, const std::string& d, double a);
    double total() const;
    double tax() const;
    void applyDiscount(double percent);
    void addLineItem(double amount);
    void markPaid();
    bool isPaid() const;
    std::string title() const override;
};

class TravelVoucher : public TravelDocument {
    bool redeemed_;
public:
    TravelVoucher(int n, const std::string& d, bool r);
    bool isRedeemed() const;
    bool canRedeem() const;
    void redeem();
    std::string title() const override;
};

class ETicket : virtual public TravelDocument {
    std::string flightCode_;
    std::string seat_;
public:
    ETicket(int n, const std::string& d, const std::string& flightCode,
            const std::string& seat);
    void reissue(const std::string& newFlightCode, const std::string& newSeat);
    bool isBoardingReady() const;
    std::string title() const override;
};

class VisaDocument : public TravelDocument {
    std::string country_;
    DateRange validity_;
public:
    VisaDocument(int n, const std::string& d, const std::string& country,
                 const DateRange& validity);
    bool covers(const DateRange& trip) const;
    bool isExpired(const std::string& date) const;
    std::string title() const override;
};

class BookingConfirmation : virtual public TravelDocument {
    const Tourist* tourist_;
    int servicesCount_;
    std::string status_;
public:
    BookingConfirmation(int n, const std::string& d, const Tourist* t, int count,
                        const std::string& status);
    int getServicesCount() const;
    void cancel();
    void confirm();
    bool isActive() const;
    std::string title() const override;
};

class InsurancePolicy : public TravelDocument {
    std::string coverageType_;
    double coverageAmount_;
public:
    InsurancePolicy(int n, const std::string& d, const std::string& coverage,
                    double coverageAmount = 50000.0);
    std::string getCoverageType() const;
    void approveClaim(double amount);
    bool covers(double damage) const;
    bool isPremium() const;
    std::string title() const override;
};