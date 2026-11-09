#pragma once

#include <string>
#include "Documents.hpp"   
#include "Basic.hpp"

class HotelPartner;        

class AgencyContract : public BookingConfirmation {
    const HotelPartner* partner_;
public:
    AgencyContract(int n, const std::string& d, int count,
                   const HotelPartner* p);
    const HotelPartner* getPartner() const;
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
    BoardingPass(const std::string& seat, const std::string& passenger,
                 const std::string& code, const Location& destination);
    std::string printPass() const;
    void checkIn();
    bool isCheckedIn() const;
    std::string getSeat() const;
};