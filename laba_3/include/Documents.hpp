#pragma once

#include <string>
#include "except.hpp"
#include "Basic.hpp"

class Tourist;
class HotelPartner;

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

class Invoice : virtual public TravelDocument {
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

class TravelVoucher : virtual public TravelDocument {
    bool redeemed_;
public:
    TravelVoucher(int n, const std::string& d, bool r);
    bool isRedeemed() const;
    bool canRedeem() const;
    void redeem();
    std::string title() const override;
};

class ETicket : public TravelDocument {
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

class BookingConfirmation : public TravelDocument {
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

class AgencyContract : public BookingConfirmation {
    const HotelPartner* partner_;
    bool signed_ = false;
public:
    AgencyContract(int n, const std::string& d, int count, const HotelPartner* p,
                   const Tourist* t);
    const HotelPartner* getPartner() const;
    void sign();
    bool isSigned() const;
    double commission() const;
    std::string title() const override;
};

class InsurancePolicy : public TravelDocument {
    std::string coverageType_;
    double coverageAmount_ = 50000.0;
public:
    InsurancePolicy(int n, const std::string& d, const std::string& coverage);
    std::string getCoverageType() const;
    void approveClaim(double amount);
    bool covers(double damage) const;
    bool isPremium() const;
    std::string title() const override;
};

class BoardingPass {
private:
    std::string seatNumber_;
    std::string passengerName_;
    std::string ticketCode_;
    bool checkedIn_;
    Location destination_;
public:
    BoardingPass(const std::string& seat,
                 const std::string& passenger,
                 const std::string& code,
                 const Location& destination);
    std::string printPass() const;
    void checkIn();
    bool isCheckedIn() const;
    std::string getSeat() const;
    bool isValidFor(const Location& location) const;
    void upgradeSeat(const std::string& newSeat);
};
