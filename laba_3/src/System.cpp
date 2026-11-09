#include "System.hpp"

#include <algorithm>



RoomBlock::RoomBlock(int number, int capacity)
    : blockNumber_(number), capacity_(capacity), occupied_(0) {
    if (blockNumber_ < 0) {
        throw TravelException("Номер блока не может быть отрицательным.");
    }
    if (capacity_ <= 0) {
        throw ReservationException("Вместимость блока должна быть положительной.");
    }
}

void RoomBlock::reserveRooms(int n) {
    if (n <= 0) {
        throw ReservationException("Количество бронируемых номеров должно быть положительным.");
    }
    if (occupied_ + n > capacity_) {
        throw FullyBookedException("В блоке недостаточно свободных номеров.");
    }
    occupied_ += n;
}

void RoomBlock::releaseRooms(int n) {
    if (n <= 0 || n > occupied_) {
        throw ReservationException("Некорректное количество освобождаемых номеров.");
    }
    occupied_ -= n;
}

void RoomBlock::addService(TravelService* service) {
    if (service == nullptr) {
        throw ReservationException("Нельзя добавить пустую услугу в блок.");
    }
    services_.push_back(service);
}

int RoomBlock::getFreeCapacity() const {
    return capacity_ - occupied_;
}

bool RoomBlock::isFullyBooked() const {
    return occupied_ >= capacity_;
}

void RoomBlock::clearBlock() {
    occupied_ = 0;
    services_.clear();
}

double RoomBlock::occupancyRate() const {
    return static_cast<double>(occupied_) / static_cast<double>(capacity_);
}


TransportCapacity::TransportCapacity(int seats, double maxWeight)
    : totalSeats_(seats), maxWeightKg_(maxWeight), currentWeight_(0.0) {
    if (totalSeats_ <= 0) {
        throw ReservationException("Количество мест должно быть положительным.");
    }
    if (maxWeightKg_ < 0.0) {
        throw ExcessBaggageException("Максимальный вес не может быть отрицательным.");
    }
}

void TransportCapacity::addBaggage(double w) {
    if (w < 0.0) {
        throw ExcessBaggageException("Вес багажа не может быть отрицательным.");
    }
    if (currentWeight_ + w > maxWeightKg_) {
        throw ExcessBaggageException("Превышена грузоподъёмность транспорта.");
    }
    currentWeight_ += w;
}

int TransportCapacity::seatLimit() const {
    return totalSeats_;
}

bool TransportCapacity::canCarryWeight(double w) const {
    return w >= 0.0 && currentWeight_ + w <= maxWeightKg_;
}

double TransportCapacity::remainingWeight() const {
    return maxWeightKg_ - currentWeight_;
}



FlightDeck::FlightDeck(int classes, int seats, double maxWeightPerClass)
    : TransportCapacity(seats, maxWeightPerClass * classes),
      travelClasses_(classes), maxWeightPerClass_(maxWeightPerClass) {
    if (travelClasses_ <= 0) {
        throw ReservationException("Количество классов обслуживания должно быть положительным.");
    }
    if (maxWeightPerClass_ < 0.0) {
        throw ExcessBaggageException("Вес на класс не может быть отрицательным.");
    }
}

bool FlightDeck::canPlaceInClass(int classIndex, double weight) const {
    return classIndex >= 0 && classIndex < travelClasses_
        && weight >= 0.0 && weight <= maxWeightPerClass_;
}

double FlightDeck::getTotalMaxWeight() const {
    return maxWeightPerClass_ * static_cast<double>(travelClasses_);
}

int FlightDeck::getClassesCount() const {
    return travelClasses_;
}

bool FlightDeck::fitsWeight(double weight) const {
    return weight >= 0.0 && currentWeight_ + weight <= maxWeightKg_;
}

void FlightDeck::addBlock(RoomBlock* block) {
    if (block == nullptr) {
        throw ReservationException("Нельзя добавить пустой блок на палубу.");
    }
    blocks_.push_back(block);
}

double FlightDeck::averageLoad() const {
    if (blocks_.empty()) {
        return 0.0;
    }
    double sum = 0.0;
    for (const RoomBlock* block : blocks_) {
        if (block != nullptr) {
            sum += block->occupancyRate();
        }
    }
    return sum / static_cast<double>(blocks_.size());
}



LuggageCompartment::LuggageCompartment(int i, double maxKg)
    : id_(i), loadKg_(0.0), maxKg_(maxKg) {
    if (id_ < 0) {
        throw TravelException("Идентификатор багажного отсека не может быть отрицательным.");
    }
    if (maxKg_ <= 0.0) {
        throw ExcessBaggageException("Максимальная загрузка отсека должна быть положительной.");
    }
}

void LuggageCompartment::loadLuggage(double kg) {
    if (kg < 0.0) {
        throw ExcessBaggageException("Вес багажа не может быть отрицательным.");
    }
    if (loadKg_ + kg > maxKg_) {
        throw ExcessBaggageException("Превышена вместимость багажного отсека.");
    }
    loadKg_ += kg;
}

bool LuggageCompartment::isFull() const {
    return loadKg_ >= maxKg_;
}

double LuggageCompartment::getLoad() const {
    return loadKg_;
}

void LuggageCompartment::unloadLuggage(double kg) {
    if (kg < 0.0 || kg > loadKg_) {
        throw ExcessBaggageException("Некорректное количество разгружаемого багажа.");
    }
    loadKg_ -= kg;
}

double LuggageCompartment::freeSpace() const {
    return maxKg_ - loadKg_;
}



DestinationRoute::DestinationRoute(int i, double length, int checkPoints)
    : id_(i), lengthKm_(length), checkpointsCount_(checkPoints), blocked_(false) {
    if (lengthKm_ <= 0.0) {
        throw TravelException("Длина маршрута должна быть положительной.");
    }
    if (checkpointsCount_ < 0) {
        throw TravelException("Количество контрольных точек не может быть отрицательным.");
    }
}

void DestinationRoute::addDeck(FlightDeck* deck) {
    if (deck == nullptr) {
        throw ReservationException("Нельзя добавить пустую палубу в маршрут.");
    }
    transportDecks_.push_back(deck);
}

int DestinationRoute::totalBlocks() const {
    int result = 0;
    for (const FlightDeck* deck : transportDecks_) {
        if (deck != nullptr) {
            result += deck->getClassesCount();
        }
    }
    return result;
}

void DestinationRoute::blockRoute() {
    blocked_ = true;
}

void DestinationRoute::unblockRoute() {
    blocked_ = false;
}

double DestinationRoute::estimatedHours(double speedKmH) const {
    if (speedKmH <= 0.0) {
        throw InvalidPriceException("Скорость должна быть положительной.");
    }
    return lengthKm_ / speedKmH;
}

bool DestinationRoute::isOperational() const {
    return !blocked_ && !transportDecks_.empty();
}



TourSection::TourSection(int i, const std::string& name, const TourCategory* cat)
    : id_(i), regionName_(name), category_(cat), capacity_(100) {
    if (regionName_.empty()) {
        throw TravelException("Название региона не может быть пустым.");
    }
}

bool TourSection::acceptsCategory(const TourCategory& cat) const {
    return category_ != nullptr && category_->matches(cat);
}

bool TourSection::hasCapacity(int groupSize) const {
    return groupSize >= 0 && groupSize <= capacity_;
}

void TourSection::reserve(int groupSize) {
    if (groupSize <= 0) {
        throw ReservationException("Размер группы должен быть положительным.");
    }
    if (groupSize > capacity_) {
        throw FullyBookedException("В секции недостаточно свободных мест.");
    }
    capacity_ -= groupSize;
}

int TourSection::freeSlots() const {
    return capacity_;
}

int TourSection::getCapacity() const {
    return capacity_;
}


VIPLounge::VIPLounge(int i, double temp, double occupancy)
    : id_(i), currentTemp_(temp), occupancyRate_(occupancy), targetTemp_(21.0) {
    if (id_ < 0) {
        throw TravelException("Идентификатор VIP-зала не может быть отрицательным.");
    }
    if (occupancyRate_ < 0.0 || occupancyRate_ > 1.0) {
        throw TravelException("Загруженность VIP-зала должна быть в диапазоне [0, 1].");
    }
}

void VIPLounge::adjustClimate() {
    constexpr double STEP = 1.0;
    if (currentTemp_ < targetTemp_) {
        currentTemp_ = std::min(currentTemp_ + STEP, targetTemp_);
    } else if (currentTemp_ > targetTemp_) {
        currentTemp_ = std::max(currentTemp_ - STEP, targetTemp_);
    }
}

bool VIPLounge::isComfortableForVip() const {
    const double diff = currentTemp_ - targetTemp_;
    return diff >= -2.0 && diff <= 2.0;
}

bool VIPLounge::needsCleaning() const {
    return occupancyRate_ >= 0.8;
}



TravelAgencySystem::TravelAgencySystem(int i, const Location& loc)
    : id_(i), mainOffice_(loc) {
    if (id_ < 0) {
        throw TravelException("Идентификатор туристической системы не может быть отрицательным.");
    }
}

void TravelAgencySystem::addSection(TourSection* section) {
    if (section == nullptr) {
        throw ReservationException("Нельзя добавить пустую секцию.");
    }
    tourSections_.push_back(section);
}

std::size_t TravelAgencySystem::getSectionsCount() const {
    return tourSections_.size();
}

std::string TravelAgencySystem::fullAddress() const {
    return name_ + ", " + mainOffice_.getFullLocation();
}

TourSection* TravelAgencySystem::findSection(const TourCategory& cat) const {
    for (TourSection* section : tourSections_) {
        if (section != nullptr && section->acceptsCategory(cat)) {
            return section;
        }
    }
    return nullptr;
}

std::string TravelAgencySystem::summary() const {
    return name_ + ": " + std::to_string(tourSections_.size()) + " секц.";
}

double TravelAgencySystem::totalCapacity() const {
    int sum = 0;
    for (const TourSection* section : tourSections_) {
        if (section != nullptr) {
            sum += section->getCapacity();
        }
    }
    return static_cast<double>(sum);
}



BookingCalendar::BookingCalendar(int maxParallel)
    : maxParallel_(maxParallel) {
    if (maxParallel_ <= 0) {
        throw ReservationException("Максимум параллельных бронирований должен быть положительным.");
    }
}

bool BookingCalendar::isFree(const DateRange& period) const {
    int overlaps = 0;
    for (const DateRange& busy : busyPeriods_) {
        if (busy.overlaps(period)) {
            ++overlaps;
        }
    }
    return overlaps < maxParallel_;
}

void BookingCalendar::addBooking(const DateRange& period) {
    if (!isFree(period)) {
        throw FullyBookedException("На выбранный период календарь заполнен.");
    }
    busyPeriods_.push_back(period);
}

int BookingCalendar::freeSlots(const DateRange& period) const {
    int overlaps = 0;
    for (const DateRange& busy : busyPeriods_) {
        if (busy.overlaps(period)) {
            ++overlaps;
        }
    }
    const int result = maxParallel_ - overlaps;
    return result > 0 ? result : 0;
}



AuditLog::AuditLog(std::size_t maxEntries)
    : maxEntries_(maxEntries) {
    if (maxEntries_ == 0) {
        throw TravelException("Максимальный размер журнала должен быть положительным.");
    }
}

void AuditLog::record(const std::string& event) {
    if (event.empty()) {
        throw TravelException("Событие не может быть пустым.");
    }
    entries_.push_back(event);
    if (entries_.size() > maxEntries_) {
        entries_.erase(entries_.begin());
    }
}

std::string AuditLog::lastEntry() const {
    return entries_.empty() ? std::string{} : entries_.back();
}

bool AuditLog::contains(const std::string& keyword) const {
    return std::any_of(entries_.begin(), entries_.end(),
                       [&keyword](const std::string& entry) {
                           return entry.find(keyword) != std::string::npos;
                       });
}