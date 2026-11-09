#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "except.hpp"
#include "Basic.hpp"

class TravelService;

class RoomBlock {
private:
    int blockNumber_;
    int capacity_;
    int occupied_;
    std::vector<TravelService*> services_;
public:
    RoomBlock(int number, int capacity);
    void reserveRooms(int n);
    void releaseRooms(int n);
    void addService(TravelService* service);
    int getFreeCapacity() const;
    bool isFullyBooked() const;
    void clearBlock();
    double occupancyRate() const;
};

class TransportCapacity {
protected:
    int totalSeats_;
    double maxWeightKg_;
    double currentWeight_ = 0.0;
public:
    TransportCapacity(int seats, double maxWeight);
    void addBaggage(double w);
    int seatLimit() const;
    bool canCarryWeight(double w) const;
    double remainingWeight() const;
};

class FlightDeck : public TransportCapacity {
private:
    int travelClasses_;
    double maxWeightPerClass_;
    std::vector<RoomBlock*> blocks_;
public:
    FlightDeck(int classes, int seats, double maxWeightPerClass);

    bool canPlaceInClass(int classIndex, double weight) const;
    double getTotalMaxWeight() const;
    int getClassesCount() const;
    bool fitsWeight(double weight) const;
    void addBlock(RoomBlock* block);
    double averageLoad() const;
};

class LuggageCompartment {
    int id_;
    double loadKg_;
    double maxKg_;
public:
    LuggageCompartment(int i, double maxKg);
    void loadLuggage(double kg);
    bool isFull() const;
    double getLoad() const;
    void unloadLuggage(double kg);
    double freeSpace() const;
};

class DestinationRoute {
    int id_;
    double lengthKm_;
    int checkpointsCount_;
    std::vector<FlightDeck*> transportDecks_;
    bool blocked_ = false;
public:
    DestinationRoute(int i, double length, int checkPoints);
    void addDeck(FlightDeck* deck);
    int totalBlocks() const;
    void blockRoute();
    void unblockRoute();
    double estimatedHours(double speedKmH) const;
    bool isOperational() const;
};

class TourSection {
    int id_;
    std::string regionName_;
    const TourCategory* category_;
    int capacity_ = 100;
public:
    TourSection(int i, const std::string& name, const TourCategory* cat);
    bool acceptsCategory(const TourCategory& cat) const;
    bool hasCapacity(int groupSize) const;
    void reserve(int groupSize);
    int freeSlots() const;
};

class VIPLounge {
    int id_;
    double currentTemp_;
    double occupancyRate_;
    double targetTemp_ = 21.0;
public:
    VIPLounge(int i, double temp, double occupancy);
    void adjustClimate();
    bool isComfortableForVip() const;
    bool needsCleaning() const;
};

class TravelAgencySystem {
    int id_;
    Location mainOffice_;
    std::vector<TourSection*> tourSections_;
    std::string name_ = "Global Travel HQ";
public:
    TravelAgencySystem(int i, const Location& loc);
    void addSection(TourSection* section);
    std::size_t getSectionsCount() const;
    std::string fullAddress() const;
    TourSection* findSection(const TourCategory& cat) const;
    std::string summary() const;
    double totalCapacity() const;
};

class BookingCalendar {
    std::vector<DateRange> busyPeriods_;
    int maxParallel_;
public:
    explicit BookingCalendar(int maxParallel);
    bool isFree(const DateRange& period) const;
    void addBooking(const DateRange& period);
    int freeSlots(const DateRange& period) const;
};

class AuditLog {
    std::vector<std::string> entries_;
    std::size_t maxEntries_;
public:
    explicit AuditLog(std::size_t maxEntries);
    void record(const std::string& event);
    std::string lastEntry() const;
    bool contains(const std::string& keyword) const;
};
