#pragma once

#include <string>
#include <vector>
#include "except.hpp"

class StorageItem;
class Customer;
class Parcel;
class Document;
class Book;
class Review;

class Stock {
    const StorageItem* item_;
    int quantity_;
    int reserved_;
public:
    Stock(const StorageItem* item, int q);
    void takeStock(int n);
    int getAvailableQuantity() const;
    void reserve(int n);
    void release(int n);
};

class Reservation {
    int id_;
    const Customer* customer_;
    bool active_;
    int expiresDay_ = 7;
public:
    Reservation(int i, const Customer* c);
    void cancelReservation();
    bool isExpired(int today) const;
    void extend(int days);
    bool isActiveReservation() const;
};

class Shipment {
    int id_;
    std::vector<const Parcel*> parcels_;
    bool sent_;
public:
    explicit Shipment(int i);
    void addParcel(const Parcel& p);
    void removeParcel(const Parcel& p);
    double totalWeight() const;
    size_t getParcelsCount() const;
    void sendShipment();
    bool isShipmentSent() const;
};

class InventoryCheck {
    int id_;
    int expected_;
    int actual_;
    std::string date_;
public:
    InventoryCheck(int i, int e, int a);
    void fixDifference(const std::string& date);
    int getDifference() const;
    bool isInventoryMatching() const;
};

class Logger {
    std::string file_name_;
    int entries_;
    int level_ = 1;
public:
    explicit Logger(const std::string& f);
    void logEntry();
    void log(const Document& d);
    bool shouldLog(int messageLevel) const;
    void clear();
    bool isLogVerbose() const;
};

class Barcode {
    std::string code_;
    int scans_;
public:
    explicit Barcode(const std::string& c);
    void incrementScans();
    int getScanCount() const;
};

class Scanner {
    int id_;
    bool on_;
    int scanned_;
    int battery_ = 100;
public:
    explicit Scanner(int i);
    void turnScannerOn();
    void turnScannerOff();
    void charge();
    void scanBarcode(Barcode& b);
};

class Rating {
    const Book* book_;
    int sum_;
    int votes_;
public:
    explicit Rating(const Book* b);
    void addVote(int v);
    void addReview(const Review& r);
    void reset();
    double getAverageRating() const;
};