#pragma once

#include <string>
#include <vector>
#include "except.hpp"
#include "Basic.hpp"

class StorageItem;


class Shelf {
private:
    int number_;
    int capacity_;
    int count_;

public:
    Shelf(int number, int capacity);
    void put(int n);
    void remove(int n);
    void putItem(const StorageItem& item);
    int getFreeSpace() const;
    bool isEmpty() const;
    void clearShelf();
};

class Container {
    int capacity_;
    double maxLoad_;
    double currentLoad_ = 0.0;
public:
    Container(int c, double m);
    void addLoad(double w);
    int volumeLimit() const;
    bool canCarry(double w) const;
};

class Rack : public Container {
private:
    int levels_;
    double maxLoadPerLevel_;
    std::vector<Shelf*> shelves_;

public:
    Rack(int levels, int capacity, double maxLoadPerLevel);

    bool canPlaceOnLevel(int level, double weight) const;
    double getTotalMaxLoad() const;
    int getLevelsCount() const;

    bool fits(double weight) const;
    void addShelf(Shelf* shelf);
};

class Pallet {
    int id_;
    double load_;
    double max_;
public:
    Pallet(int i, double m);
    void load(double kg);
    bool isFull() const;
    double getLoad() const;
    void unload(double kg);
};

class Aisle {
    int id_;
    double length_;
    int shelvesCount_;
    std::vector<Rack*> racks_;
    bool blocked_ = false;
public:
    Aisle(int i, double l, int s);
    void addRack(Rack* rack);
    int totalShelves() const;
    void block();
    void unblock();
};

class Section {
    int id_;
    std::string name_;
    const Genre* genre_;
    int capacity_ = 100;
public:
    Section(int i, const std::string& n, const Genre* g);
    bool accepts(const Genre& g) const;
    bool hasSpace(int booksCount) const;
};

class ClimateRoom {
    int id_;
    double temperature_;
    double humidity_;
    double targetTemp_ = 18.0;
public:
    ClimateRoom(int i, double t, double h);
    void adjustTemperature();
    bool isSafeForRareBooks() const;
};

class Warehouse {
    int id_;
    Address address_;
    std::vector<Section*> sections_;
    std::string name_ = "Main";
public:
    Warehouse(int i, const Address& a);
    void addSection(Section* section);
    size_t getSectionsCount() const;
    std::string fullAddress() const;
    Section* findSection(const Genre& g) const;
    std::string summary() const;
};