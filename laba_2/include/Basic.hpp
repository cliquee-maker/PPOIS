#pragma once

#include <string>
#include "except.hpp"

class Address{
private:
    std::string city_;
    std::string street_;
    std::string zip_;
    std::string country_;
public:
    Address(const std::string country, const std::string& c, const std::string& s, const std::string& z);
    std::string getAdress() const;
    std::string getCountry() const;
};

class Price{
private:
    double price_;
    std::string currency_;
public:
    Price (double p, const std::string currency);
    double getPriceWithNDS(int NDS = 20) const;
    double getPriceWithSale(int Sale) const;
    Price convertTo(double rate, const std::string& currency) const;
    Price add(const Price& other) const;
};

class Dimensions {
private:
    double high_;
    double length_;
    double width_;
    double weight_;
public:
    Dimensions (double h, double l, double w, double weight);
    double getWeight() const;
    double getWidth() const; 
    double getHigh() const;
    double getLength() const; 
    double getVolume() const; 
    bool fitsIn(double cell) const;
};

class Genre {
private:
    int id_;
    std::string name_;
    int ageLimit_;
public:
    Genre(int id, const std::string& name, int ageLimit);
    std::string getGenre() const;
    int getAge() const;
    bool allowedFor (int age) const;
};

class Author {
private:
    int id_;
    std::string name_;
    int birthYear_;
    std::string country_ = "Unknown";
public:
    Author(int i, const std::string& n, int b);
    std::string getAuthor() const;
    int age(int currentYear) const;
    void setCountry(const std::string& country);
    bool isContemporary(int currentYear) const;
};

class Publisher {
private:
    int id_;
    std::string name_;
    Address address_;
    int foundedYear_;
public:
    Publisher(int i, const std::string& n, const Address& a);
    std::string getPublisher() const;
    void setFoundedYear(int year);
    int yearsOnMarket(int currentYear) const;
};