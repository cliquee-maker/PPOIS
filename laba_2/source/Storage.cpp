#include "Storage.hpp"
#include "Items.hpp"
#include <algorithm>


Shelf::Shelf(int number, int capacity)
    : number_(number), capacity_(capacity), count_(0)
{
    if (capacity <= 0) throw InvalidQuantityException("Вместимость полки должна быть положительной");
}

void Shelf::put(int n) {
    if (n <= 0) throw InvalidQuantityException("Количество должно быть положительным");
    if (count_ + n > capacity_) throw ShelfOverflowException("Недостаточно места на полке");
    count_ += n;
}

void Shelf::remove(int n) {
    if (n <= 0 || n > count_) throw InvalidQuantityException("Некорректное количество товара");
    count_ -= n;
}

void Shelf::putItem(const StorageItem& item) {
    if (item.weight() <= 0) throw InvalidDimensionsException("У товара некорректный вес");
    put(1);
}

int Shelf::getFreeSpace() const {
    return capacity_ - count_;
}

bool Shelf::isEmpty() const {
    return count_ == 0;
}

void Shelf::clearShelf() {
    count_ = 0;
}


Container::Container(int c, double m) : capacity_(c), maxLoad_(m) {}

void Container::addLoad(double w) {
    if (w <= 0) throw InvalidQuantityException("Вес должен быть больше нуля");
    if (!canCarry(currentLoad_ + w)) throw ShelfOverflowException("Контейнер перегружен");
    currentLoad_ += w;
}

int Container::volumeLimit() const {
    return capacity_;
}

bool Container::canCarry(double w) const {
    return w <= maxLoad_;
}


Rack::Rack(int levels, int capacity, double maxLoadPerLevel)
    : Container(capacity, maxLoadPerLevel * levels),
      levels_(levels),
      maxLoadPerLevel_(maxLoadPerLevel)
{
    if (levels <= 0) {
        throw InvalidQuantityException("Количество уровней стеллажа должно быть положительным");
    }
    if (maxLoadPerLevel <= 0) {
        throw InvalidDimensionsException("Максимальная нагрузка должна быть положительной");
    }
}

bool Rack::canPlaceOnLevel(int level, double weight) const {
    if (level < 1 || level > levels_) return false;
    return weight > 0 && weight <= maxLoadPerLevel_;
}

double Rack::getTotalMaxLoad() const {
    return levels_ * maxLoadPerLevel_;
}

int Rack::getLevelsCount() const {
    return levels_;
}

bool Rack::fits(double weight) const {
    return weight > 0 && weight <= getTotalMaxLoad();
}

void Rack::addShelf(Shelf* shelf) {
    if (static_cast<int>(shelves_.size()) >= levels_) {
        throw ShelfOverflowException("На стеллаже нет свободных уровней");
    }
    shelves_.push_back(shelf);
}


Pallet::Pallet(int i, double m) : id_(i), load_(0), max_(m) {}

void Pallet::load(double kg) {
    if (kg <= 0) throw InvalidQuantityException("Вес должен быть больше нуля");
    if (load_ + kg > max_) throw ShelfOverflowException("pallet overloaded");
    load_ += kg;
}

bool Pallet::isFull() const {
    return load_ >= max_;
}

double Pallet::getLoad() const {
    return load_;
}

void Pallet::unload(double kg) {
    if (kg <= 0 || kg > load_) throw InvalidQuantityException("Нельзя снять такой вес");
    load_ -= kg;
}


Aisle::Aisle(int i, double l, int s) : id_(i), length_(l), shelvesCount_(s) {}

void Aisle::addRack(Rack* rack) {
    if (blocked_) throw WarehouseException("Проход закрыт");
    racks_.push_back(rack);
}

int Aisle::totalShelves() const {
    return shelvesCount_ * 2;
}

void Aisle::block() {
    blocked_ = true;
}

void Aisle::unblock() {
    blocked_ = false;
}


Section::Section(int i, const std::string& n, const Genre* g) : id_(i), name_(n), genre_(g) {}

bool Section::accepts(const Genre& g) const {
    return genre_ == &g || genre_->getGenre() == g.getGenre();
}

bool Section::hasSpace(int booksCount) const {
    return booksCount < capacity_;
}


ClimateRoom::ClimateRoom(int i, double t, double h) : id_(i), temperature_(t), humidity_(h) {}

void ClimateRoom::adjustTemperature() {
    if (temperature_ < targetTemp_)      
     temperature_ = std::min(temperature_ + 1.0, targetTemp_);
    else if 
     (temperature_ > targetTemp_) temperature_ = std::max(temperature_ - 1.0, targetTemp_);
}

bool ClimateRoom::isSafeForRareBooks() const {
    return temperature_ >= 16 && temperature_ <= 20 && humidity_ >= 40 && humidity_ <= 55;
}


Warehouse::Warehouse(int i, const Address& a) : id_(i), address_(a) {}

void Warehouse::addSection(Section* section) {
    sections_.push_back(section);
}

size_t Warehouse::getSectionsCount() const {
    return sections_.size();
}

std::string Warehouse::fullAddress() const {
    return address_.getAdress();
}

Section* Warehouse::findSection(const Genre& g) const {
    for (Section* s : sections_) {
        if (s->accepts(g)) return s;
    }
    return nullptr;
}

std::string Warehouse::summary() const {
    return name_ + ": " + fullAddress() + ", секций: " + std::to_string(sections_.size());
}