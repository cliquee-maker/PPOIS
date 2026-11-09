#include "Review.hpp"

AgencyContract::AgencyContract(int n, const std::string& d, int count,
                               const HotelPartner* p)
    : TravelDocument(n, d),                                   
      BookingConfirmation(n, d, nullptr, count, "pending"),
      partner_(p) {}

const HotelPartner* AgencyContract::getPartner() const {
    return partner_;
}

std::string AgencyContract::title() const {
    return "Агентский договор №" + std::to_string(documentNumber_);
}


BoardingPass::BoardingPass(const std::string& seat, const std::string& passenger,
                           const std::string& code, const Location& destination)
    : seatNumber_(seat), passengerName_(passenger), ticketCode_(code),
      checkedIn_(false), destination_(destination) {
    if (seat.empty() || passenger.empty() || code.empty()) {
        throw TravelException("Параметры посадочного талона не могут быть пустыми.");
    }
}

std::string BoardingPass::printPass() const {
    return "Посадочный талон: " + passengerName_
         + ", Место: " + seatNumber_
         + ", Код: " + ticketCode_;
}

void BoardingPass::checkIn() {
    if (checkedIn_) {
        throw ReservationException("Регистрация на рейс уже пройдена.");
    }
    checkedIn_ = true;
}

bool BoardingPass::isCheckedIn() const {
    return checkedIn_;
}

std::string BoardingPass::getSeat() const {
    return seatNumber_;
}