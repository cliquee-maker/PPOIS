#include "Items.hpp"


StorageItem::StorageItem(int i, const std::string& t, int q)
    : id_(i), title_(t), quantity_(q)
{
    if (q < 0) throw InvalidQuantityException("Количество не может быть отрицательным");
}

StorageItem::~StorageItem() {}

void StorageItem::setPrice(double price) {
    if (price < 0) throw InvalidPriceException("Цена не может быть отрицательной");
    price_ = price;
}

double StorageItem::totalValue() const {
    return price_ * quantity_;
}

void StorageItem::restock(int n) {
    if (n <= 0) throw InvalidQuantityException("Количество должно быть больше нуля");
    quantity_ += n;
}

void StorageItem::sell(int n) {
    if (n <= 0) throw InvalidQuantityException("Количество должно быть больше нуля");
    if (n > quantity_) throw OutOfStockException("Недостаточно товара: " + title_);
    quantity_ -= n;
}

std::string StorageItem::describe() const {
    return title_ + " x" + std::to_string(quantity_);
}


Book::Book(int id,
           const std::string& title,
           int quantity,
           int pages,
           const std::string& isbn,
           const Author& author,
           const Publisher& publisher,
           const Genre& genre)
    : StorageItem(id, title, quantity),
      pages_(pages),
      isbn_(isbn),
      author_(author),
      publisher_(publisher),
      genre_(genre)
{
    if (pages <= 0) throw InvalidQuantityException("Количество страниц должно быть положительным");
}

double Book::weight() const {
    return pages_ * 0.0006;
}

std::string Book::describe() const {
    return StorageItem::describe() + " (" + std::to_string(pages_) + " pages, "
         + author_.getAuthor() + ", " + publisher_.getPublisher() + ", "
         + genre_.getGenre() + ")";
}

bool Book::isWrittenBy(const Author& author) const {
    return &author_ == &author || author_.getAuthor() == author.getAuthor();
}

bool Book::isAllowedFor(int age) const {
    return genre_.allowedFor(age);
}

void Book::setYear(int year) {
    if (year <= 0) throw WarehouseException("Некорректный год издания");
    year_ = year;
}

bool Book::isClassic(int currentYear) const {
    return currentYear - year_ >= 50;
}


Hardcover::Hardcover(int i, const std::string& t, int q, int p,
                     const std::string& isbn, const Author& a,
                     const Publisher& pub, const Genre& g, double cw)
    : Book(i, t, q, p, isbn, a, pub, g), coverWeight_(cw) {}

double Hardcover::weight() const {
    return Book::weight() + coverWeight_;
}

std::string Hardcover::describe() const {
    return Book::describe() + " [hardcover]";
}


Paperback::Paperback(int i, const std::string& t, int q, int p,
                     const std::string& isbn, const Author& a,
                     const Publisher& pub, const Genre& g, bool glued)
    : Book(i, t, q, p, isbn, a, pub, g), glued_(glued) {}

bool Paperback::isGlued() const {
    return glued_;
}

std::string Paperback::describe() const {
    return Book::describe() + " [paperback]";
}


Atlas::Atlas(int i, const std::string& t, int q, int p,
             const std::string& isbn, const Author& a,
             const Publisher& pub, const Genre& g, int m)
    : Book(i, t, q, p, isbn, a, pub, g), maps_(m) {}

double Atlas::weight() const {
    return Book::weight() * 1.5;
}

std::string Atlas::describe() const {
    return Book::describe() + " [atlas, maps: " + std::to_string(maps_) + "]";
}


Comic::Comic(int i, const std::string& t, int q, int p,
             const std::string& isbn, const Author& a,
             const Publisher& pub, const Genre& g, bool c)
    : Book(i, t, q, p, isbn, a, pub, g), colored_(c) {}

double Comic::weight() const {
    return Book::weight() * 0.8;
}

std::string Comic::describe() const {
    return Book::describe() + (colored_ ? " [color comic]" : " [bw comic]");
}


Magazine::Magazine(int i, const std::string& t, int q, int is, int pages)
    : StorageItem(i, t, q), issue_(is), pages_(pages) {}

double Magazine::weight() const {
    return pages_ * 0.003;
}

std::string Magazine::describe() const {
    return StorageItem::describe() + " #" + std::to_string(issue_);
}


AudioBook::AudioBook(int i, const std::string& t, int q, int m, int d)
    : StorageItem(i, t, q), minutes_(m), discs_(d) {}

double AudioBook::weight() const {
    return discs_ * 0.1;
}

std::string AudioBook::describe() const {
    return StorageItem::describe() + " [" + std::to_string(minutes_) + " min]";
}