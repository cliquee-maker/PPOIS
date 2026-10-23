#include "Operation.hpp"
#include "Documents.hpp"
#include "Review.hpp"
#include <algorithm>
#include <fstream>


Stock::Stock(const StorageItem* item, int q) : item_(item), quantity_(q), reserved_(0) {
    if (q < 0) throw InvalidQuantityException("Количество не может быть отрицательным");
}

void Stock::takeStock(int n) {
    if (n <= 0) {
        throw InvalidQuantityException("Количество списываемого товара должно быть положительным");
    }
    if (n > quantity_ - reserved_) {
        throw OutOfStockException("Недостаточно товара на складе");
    }
    quantity_ -= n;
}

int Stock::getAvailableQuantity() const {
    return quantity_ - reserved_;
}

void Stock::reserve(int n) {
    if (n <= 0) throw InvalidQuantityException("Количество должно быть больше нуля");
    if (n > getAvailableQuantity()) throw ReservationException("Недостаточно свободного товара");
    reserved_ += n;
}

void Stock::release(int n) {
    if (n <= 0 || n > reserved_) throw ReservationException("Нельзя снять такой резерв");
    reserved_ -= n;
}


Reservation::Reservation(int i, const Customer* c) : id_(i), customer_(c), active_(true) {
    if (c == nullptr) throw ReservationException("Бронь должна быть на покупателя");
}

void Reservation::cancelReservation() {
    if (!active_) throw ReservationException("Бронь уже отменена");
    active_ = false;
}

bool Reservation::isExpired(int today) const {
    return today > expiresDay_;
}

void Reservation::extend(int days) {
    if (!active_) throw ReservationException("Бронь уже отменена");
    if (days <= 0) throw InvalidQuantityException("Срок продления должен быть больше нуля");
    expiresDay_ += days;
}

bool Reservation::isActiveReservation() const {
    return active_;
}


Shipment::Shipment(int i) : id_(i), sent_(false) {}

void Shipment::addParcel(const Parcel& p) {
    if (sent_) throw ShipmentAlreadySentException("Нельзя добавить посылку: отправка уже ушла");
    parcels_.push_back(&p);
}

void Shipment::removeParcel(const Parcel& p) {
    if (sent_) throw ShipmentAlreadySentException("Нельзя убрать посылку: отправка уже ушла");
    auto it = std::find(parcels_.begin(), parcels_.end(), &p);
    if (it != parcels_.end()) parcels_.erase(it);
}

double Shipment::totalWeight() const {
    double sum = 0;
    for (const Parcel* p : parcels_) sum += p->getWeight();
    return sum;
}

size_t Shipment::getParcelsCount() const {
    return parcels_.size();
}

void Shipment::sendShipment() {
    if (sent_) throw ShipmentAlreadySentException("Shipment is already sent!");
    if (parcels_.empty()) throw InvalidQuantityException("В отправке нет посылок");
    sent_ = true;
}

bool Shipment::isShipmentSent() const {
    return sent_;
}


InventoryCheck::InventoryCheck(int i, int e, int a) : id_(i), expected_(e), actual_(a) {}

void InventoryCheck::fixDifference(const std::string& date) {
    expected_ = actual_;  
    date_ = date;
}

int InventoryCheck::getDifference() const {
    return actual_ - expected_;
}

bool InventoryCheck::isInventoryMatching() const {
    return expected_ == actual_;
}


Logger::Logger(const std::string& f) : file_name_(f), entries_(0) {}

void Logger::logEntry() {
    entries_++;
}

void Logger::log(const Document& d) {
    std::ofstream out(file_name_, std::ios::app);
    out << "#" << d.getNumber() << " " << d.title() << "\n";
    logEntry();
}

bool Logger::shouldLog(int messageLevel) const {
    return messageLevel >= level_;
}

void Logger::clear() {
    std::ofstream out(file_name_, std::ios::trunc);
    entries_ = 0;
}

bool Logger::isLogVerbose() const {
    return entries_ > 1000;
}


Barcode::Barcode(const std::string& c) : code_(c), scans_(0) {}

void Barcode::incrementScans() {
    scans_++;
}

int Barcode::getScanCount() const {
    return scans_;
}


Scanner::Scanner(int i) : id_(i), on_(false), scanned_(0) {}

void Scanner::turnScannerOn() {
    if (battery_ <= 0) throw ScannerOfflineException("Батарея сканера разряжена");
    on_ = true;
}

void Scanner::turnScannerOff() {
    on_ = false;
}

void Scanner::charge() {
    battery_ = 100;
}

void Scanner::scanBarcode(Barcode& b) {
    if (!on_) throw ScannerOfflineException("Cannot scan: scanner is turned off!");
    if (battery_ <= 0) {
        on_ = false;
        throw ScannerOfflineException("Батарея сканера разряжена");
    }
    b.incrementScans();
    scanned_++;
    battery_--;
}


Rating::Rating(const Book* b) : book_(b), sum_(0), votes_(0) {
    if (b == nullptr) throw InvalidRatingException("Рейтинг должен относиться к книге");
}

void Rating::addVote(int v) {
    if (v < 1 || v > 5) throw InvalidRatingException("Rating vote must be between 1 and 5!");
    sum_ += v;
    ++votes_;
}

void Rating::addReview(const Review& r) {
    addVote(r.getRating());
}

void Rating::reset() {
    sum_ = 0;
    votes_ = 0;
}

double Rating::getAverageRating() const {
    return votes_ == 0 ? 0.0 : static_cast<double>(sum_) / votes_;
}