#pragma once

#include <stdexcept>
#include <string>

class WarehouseException : public std::runtime_error {
public:
    explicit WarehouseException(const std::string& message)
        : std::runtime_error(message) {}
};

class InvalidPriceException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class OutOfStockException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class ReservationException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class SalaryException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class ShelfOverflowException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class PaymentFailedException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class AgeRestrictionException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class ScannerOfflineException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class InvalidDimensionsException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class InvalidRatingException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class ShipmentAlreadySentException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class InvalidQuantityException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class InvalidPromoCodeException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};

class InvalidDiscountException : public WarehouseException {
public:
    using WarehouseException::WarehouseException;
};