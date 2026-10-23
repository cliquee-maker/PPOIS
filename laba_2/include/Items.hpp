#pragma once

#include <string>
#include "Basic.hpp"
#include "except.hpp"

class StorageItem {
protected:
    int id_;
    std::string title_;
    int quantity_;
    double price_ = 0.0;
public:
    StorageItem(int i, const std::string& t, int q);
    virtual ~StorageItem();
    void setPrice(double price);
    double totalValue() const;
    void restock(int n);
    void sell(int n);
    virtual double weight() const = 0;
    virtual std::string describe() const;
};

class Book : public StorageItem {
protected:
    int pages_;
private:
    std::string isbn_;
    const Author& author_;
    const Publisher& publisher_;
    const Genre& genre_;
    int year_ = 2000;
public:
    Book(int id,
         const std::string& title,
         int quantity,
         int pages,
         const std::string& isbn,
         const Author& author,
         const Publisher& publisher,
         const Genre& genre);

    double weight() const override;
    std::string describe() const override;

    bool isWrittenBy(const Author& author) const;
    bool isAllowedFor(int age) const;
    void setYear(int year);
    bool isClassic(int currentYear) const;
};

class Hardcover : public Book {
    double coverWeight_;
public:
    Hardcover(int i, const std::string& t, int q, int p,
              const std::string& isbn, const Author& a,
              const Publisher& pub, const Genre& g, double cw);
    double weight() const override;
    std::string describe() const override;
};

class Paperback : public Book {
    bool glued_;
public:
    Paperback(int i, const std::string& t, int q, int p,
              const std::string& isbn, const Author& a,
              const Publisher& pub, const Genre& g, bool glued);
    bool isGlued() const;
    std::string describe() const override;
};

class Atlas : public Book {
    int maps_;
public:
    Atlas(int i, const std::string& t, int q, int p,
          const std::string& isbn, const Author& a,
          const Publisher& pub, const Genre& g, int m);
    double weight() const override;
    std::string describe() const override;
};

class Comic : public Book {
    bool colored_;
public:
    Comic(int i, const std::string& t, int q, int p,
          const std::string& isbn, const Author& a,
          const Publisher& pub, const Genre& g, bool c);
    double weight() const override;
    std::string describe() const override;
};

class Magazine : public StorageItem {
    int issue_;
    int pages_;
public:
    Magazine(int i, const std::string& t, int q, int is, int pages);
    double weight() const override;
    std::string describe() const override;
};

class AudioBook : public StorageItem {
    int minutes_;
    int discs_;
public:
    AudioBook(int i, const std::string& t, int q, int m, int d);
    double weight() const override;
    std::string describe() const override;
};