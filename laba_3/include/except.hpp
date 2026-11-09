#pragma once

#include <stdexcept>
#include <string>

class TravelException : public std::runtime_error {
public:
    explicit TravelException(const std::string& message)
        : std::runtime_error(message) {}
};

class InvalidPriceException : public TravelException {
public:
    using TravelException::TravelException;
};

class FullyBookedException : public TravelException {
public:
    using TravelException::TravelException;
};

class ReservationException : public TravelException {
public:
    using TravelException::TravelException;
};

class PaymentFailedException : public TravelException {
public:
    using TravelException::TravelException;
};

class InvalidPromoCodeException : public TravelException {
public:
    using TravelException::TravelException;
};

class InvalidPassportException : public TravelException {
public:
    using TravelException::TravelException;
};

class TransportCancelledException : public TravelException {
public:
    using TravelException::TravelException;
};

class ExcessBaggageException : public TravelException {
public:
    using TravelException::TravelException;
};

class VisaRequiredException : public TravelException {
public:
    using TravelException::TravelException;
};

class InvalidDateRangeException : public TravelException {
public:
    using TravelException::TravelException;
};

class DuplicateBookingException : public TravelException {
public:
    using TravelException::TravelException;
};

class RefundNotAllowedException : public TravelException {
public:
    using TravelException::TravelException;
};

class UnauthorizedAccessException : public TravelException {
public:
    using TravelException::TravelException;
};
