#include <UnitTest++/UnitTest++.h>

#include "Basic.hpp"
#include "Cart.hpp"
#include "Documents.hpp"
#include "Items.hpp"
#include "Operation.hpp"
#include "People.hpp"
#include "Review.hpp"
#include "Storage.hpp"
#include "except.hpp"

SUITE(AddressSuite)
{
    TEST(Address_GetAdress)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        CHECK_EQUAL("Russia, Moscow, Tverskaya 123456", addr.getAdress());
    }

    TEST(Address_GetCountry)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        CHECK_EQUAL("Russia", addr.getCountry());
    }
}

SUITE(PriceSuite)
{
    TEST(Price_NDS)
    {
        Price p(100.0, "RUB");
        CHECK_CLOSE(120.0, p.getPriceWithNDS(20), 0.001);
    }

    TEST(Price_Sale)
    {
        Price p(100.0, "RUB");
        CHECK_CLOSE(80.0, p.getPriceWithSale(20), 0.001);
    }

    TEST(Price_ConvertAndAdd)
    {
        Price p(100.0, "RUB");
        Price p2 = p.convertTo(1.5, "USD");
        CHECK_CLOSE(150.0, p2.getPriceWithNDS(0), 0.001);
        
        Price p3 = p.add(Price(50.0, "RUB"));
        CHECK_CLOSE(150.0, p3.getPriceWithNDS(0), 0.001);
    }

    TEST(Price_Throws)
    {
        CHECK_THROW(Price(-10, "RUB"), InvalidPriceException);
        Price p(100.0, "RUB");
        CHECK_THROW(p.getPriceWithSale(-5), InvalidDiscountException);
        CHECK_THROW(p.convertTo(0, "USD"), InvalidPriceException);
        CHECK_THROW(p.add(Price(10, "USD")), InvalidPriceException);
    }
}

SUITE(DimensionsSuite)
{
    TEST(Dimensions_VolumeAndWeight)
    {
        Dimensions d(2.0, 3.0, 4.0, 1.5);
        CHECK_CLOSE(24.0, d.getVolume(), 0.001);
        CHECK_CLOSE(1.5, d.getWeight(), 0.001);
        CHECK(d.fitsIn(25.0));
        CHECK(!d.fitsIn(23.0));
    }

    TEST(Dimensions_Throws)
    {
        CHECK_THROW(Dimensions(-1, 1, 1, 1), InvalidDimensionsException);
    }
}

SUITE(GenreSuite)
{
    TEST(Genre_AgeLimit)
    {
        Genre g(1, "Horror", 18);
        CHECK_EQUAL("Horror", g.getGenre());
        CHECK_EQUAL(18, g.getAge());
        CHECK(g.allowedFor(18));
        CHECK(!g.allowedFor(17));
        CHECK_THROW(g.allowedFor(-1), AgeRestrictionException);
    }

    TEST(Genre_Throws)
    {
        CHECK_THROW(Genre(1, "Horror", -1), AgeRestrictionException);
    }
}

SUITE(AuthorSuite)
{
    TEST(Author_AgeAndCountry)
    {
        Author a(1, "King", 1947);
        CHECK_EQUAL("King", a.getAuthor());
        CHECK_EQUAL(76, a.age(2023));
        CHECK(a.isContemporary(2023));
        a.setCountry("USA");
    }
}

SUITE(PublisherSuite)
{
    TEST(Publisher_YearsOnMarket)
    {
        Address addr("USA", "NY", "5th", "10001");
        Publisher p(1, "AST", addr);
        CHECK_EQUAL("AST", p.getPublisher());
        p.setFoundedYear(2000);
        CHECK_EQUAL(23, p.yearsOnMarket(2023));
        CHECK_THROW(p.setFoundedYear(0), WarehouseException);
    }
}

SUITE(PromoCodeSuite)
{
    TEST(PromoCode_ApplyAndUse)
    {
        PromoCode pc("SALE", 10);
        CHECK(pc.isActive());
        CHECK_CLOSE(90.0, pc.apply(100.0), 0.001);
        pc.use();
        pc.use();
        pc.use();
        pc.use();
        pc.use();
        pc.use();
        pc.use();
        pc.use();
        pc.use();
        pc.use();
        CHECK(!pc.isActive());
        pc.deactivate();
    }

    TEST(PromoCode_Throws)
    {
        CHECK_THROW(PromoCode("", 10), InvalidPromoCodeException);
        CHECK_THROW(PromoCode("SALE", -10), InvalidPromoCodeException);
    }
}

SUITE(CartSuite)
{
    TEST(Cart_AddRemoveClear)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Customer customer(1, "Ivan", addr);
        Cart cart(&customer);
        
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Book book(1, "It", 10, 500, "123", author, pub, genre);

        cart.addItem(&book);
        CHECK_EQUAL(1, cart.getItemsCount());
        CHECK(cart.contains(&book));
        
        cart.removeItem(&book);
        CHECK_EQUAL(0, cart.getItemsCount());
        
        cart.addItem(&book);
        cart.clear();
        CHECK_EQUAL(0, cart.getItemsCount());
    }

    TEST(Cart_Throws)
    {
        Cart cart(nullptr);
        CHECK_THROW(cart.addItem(nullptr), InvalidQuantityException);
        CHECK_THROW(cart.removeItem(nullptr), OutOfStockException);
    }
}

SUITE(OrderItemSuite)
{
    TEST(OrderItem_Total)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Book book(1, "It", 10, 500, "123", author, pub, genre);

        OrderItem item(&book, 2, Price(100.0, "RUB"));
        CHECK_CLOSE(200.0, item.getTotal(), 0.001);
        
        item.changeQuantity(3);
        CHECK_CLOSE(300.0, item.getTotal(), 0.001);
        
        CHECK_THROW(item.changeQuantity(0), InvalidQuantityException);
    }
}

SUITE(PaymentSuite)
{
    TEST(CardPayment_Process)
    {
        CardPayment cp(500.0, "4111111111111111");
        CHECK(cp.processPayment());
    }

    TEST(CardPayment_Throws)
    {
        CardPayment cp(500.0, "");
        CHECK_THROW(cp.processPayment(), PaymentFailedException);
    }

    TEST(CashPayment_ProcessAndChange)
    {
        CashPayment cp(500.0, 1000.0);
        CHECK(cp.processPayment());
        CHECK_CLOSE(500.0, cp.getChange(), 0.001);
    }

    TEST(CashPayment_Throws)
    {
        CashPayment cp(500.0, 100.0);
        CHECK_THROW(cp.processPayment(), PaymentFailedException);
        CHECK_THROW(cp.getChange(), PaymentFailedException);
    }

    TEST(Payment_Refund)
    {
        CashPayment cp(500.0, 500.0);
        cp.processPayment();
        cp.refund("2023-10-24");
        CHECK_THROW(cp.refund("2023-10-25"), PaymentFailedException);
    }
}

SUITE(DocumentSuite)
{
    TEST(Document_Sign)
    {
        class TempDoc : public Document {
        public: TempDoc() : Document(1, "date") {}
        } doc;
        
        doc.sign();
        CHECK_THROW(doc.sign(), WarehouseException);
        CHECK_EQUAL(1, doc.getNumber());
        CHECK_EQUAL("document", doc.title());
    }
}

SUITE(InvoiceSuite)
{
    TEST(Invoice_TotalAndTax)
    {
        Invoice inv(1, "date", 100.0);
        CHECK_CLOSE(120.0, inv.total(), 0.001);
        inv.applyDiscount(50);
        CHECK_CLOSE(60.0, inv.total(), 0.001);
        CHECK_EQUAL("invoice", inv.title());
    }

    TEST(Invoice_Throws)
    {
        CHECK_THROW(Invoice(1, "date", -10), InvalidPriceException);
        Invoice inv(1, "date", 100.0);
        CHECK_THROW(inv.applyDiscount(-10), InvalidDiscountException);
    }
}

SUITE(ReceiptSuite)
{
    TEST(Receipt_IsPaid)
    {
        Receipt rec(1, "date", true);
        CHECK(rec.isPaid());
        CHECK_EQUAL("receipt", rec.title());
    }
}

SUITE(OrderSuite)
{
    TEST(Order_CancelAndTitle)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Customer c(1, "Ivan", addr);
        Order o(1, "date", &c, 5, "new");
        
        CHECK_EQUAL(5, o.getItemsCount());
        o.cancel();
        CHECK_EQUAL("order of customer Ivan", o.title());
    }

    TEST(Order_Throws)
    {
        CHECK_THROW(Order(1, "date", nullptr, -1, "new"), InvalidQuantityException);
    }
}

SUITE(PurchaseOrderSuite)
{
    TEST(PurchaseOrder_Title)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Supplier s(1, "Supplier", addr, "Company");
        PurchaseOrder po(1, "date", 5, &s);
        CHECK_EQUAL("purchase order from Supplier", po.title());
        CHECK(po.getSupplier() == &s);
    }
}

SUITE(ReturnNoteSuite)
{
    TEST(ReturnNote_ReasonAndApprove)
    {
        ReturnNote rn(1, "date", "broken");
        CHECK_EQUAL("broken", rn.getReason());
        rn.approve(100.0);
        CHECK_THROW(rn.approve(-10), InvalidPriceException);
        CHECK_EQUAL("return note", rn.title());
    }
}

SUITE(ParcelSuite)
{
    TEST(Parcel_Lifecycle)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Parcel p(1.5, "books", "TRACK123", addr);
        
        CHECK_CLOSE(1.5, p.getWeight(), 0.001);
        CHECK(!p.isDelivered());
        p.markDelivered();
        CHECK(p.isDelivered());
        CHECK(p.trackingLabel().find("TRACK123") != std::string::npos);
    }

    TEST(Parcel_Throws)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        CHECK_THROW(Parcel(-1, "books", "TRACK123", addr), InvalidDimensionsException);
        CHECK_THROW(Parcel(1, "books", "", addr), std::invalid_argument);
    }
}

SUITE(StorageItemSuite)
{
    TEST(StorageItem_SellRestockValue)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Book book(1, "It", 10, 500, "123", author, pub, genre);

        book.setPrice(100.0);
        CHECK_CLOSE(1000.0, book.totalValue(), 0.001);

        book.sell(2);
        CHECK_CLOSE(800.0, book.totalValue(), 0.001);

        book.restock(5);
        CHECK_CLOSE(1300.0, book.totalValue(), 0.001);
    }

    TEST(StorageItem_Throws)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        
        CHECK_THROW(Book(1, "It", -1, 500, "123", author, pub, genre), InvalidQuantityException);
        
        Book book(1, "It", 10, 500, "123", author, pub, genre);
        CHECK_THROW(book.sell(20), OutOfStockException);
        CHECK_THROW(book.restock(-1), InvalidQuantityException);
        CHECK_THROW(book.setPrice(-10), InvalidPriceException);
    }
}

SUITE(BookSuite)
{
    TEST(Book_WeightAndLogic)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Book book(1, "It", 10, 500, "123", author, pub, genre);

        CHECK_CLOSE(0.3, book.weight(), 0.001);
        CHECK(book.isWrittenBy(author));
        CHECK(book.isAllowedFor(18));
        
        book.setYear(2000);
        CHECK(book.isClassic(2050));
        
        CHECK_THROW(book.setYear(0), WarehouseException);
    }
}

SUITE(HardcoverSuite)
{
    TEST(Hardcover_Weight)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Hardcover hc(1, "It", 10, 500, "123", author, pub, genre, 0.2);
        
        CHECK_CLOSE(0.5, hc.weight(), 0.001);
    }
}

SUITE(PaperbackSuite)
{
    TEST(Paperback_Glued)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Paperback pb(1, "It", 10, 500, "123", author, pub, genre, true);
        
        CHECK(pb.isGlued());
    }
}

SUITE(AtlasSuite)
{
    TEST(Atlas_Weight)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Atlas a(1, "Maps", 10, 500, "123", author, pub, genre, 50);
        
        CHECK_CLOSE(0.45, a.weight(), 0.001);
    }
}

SUITE(ComicSuite)
{
    TEST(Comic_Weight)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Comic c(1, "Comic", 10, 500, "123", author, pub, genre, true);
        
        CHECK_CLOSE(0.24, c.weight(), 0.001);
    }
}

SUITE(MagazineSuite)
{
    TEST(Magazine_Weight)
    {
        Magazine m(1, "Vogue", 10, 5, 100);
        CHECK_CLOSE(0.3, m.weight(), 0.001);
    }
}

SUITE(AudioBookSuite)
{
    TEST(AudioBook_Weight)
    {
        AudioBook a(1, "It", 10, 600, 2);
        CHECK_CLOSE(0.2, a.weight(), 0.001);
    }
}

SUITE(StockSuite)
{
    TEST(Stock_TakeReserveRelease)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Book book(1, "It", 10, 500, "123", author, pub, genre);

        Stock s(&book, 10);
        s.reserve(3);
        CHECK_EQUAL(7, s.getAvailableQuantity());
        
        s.takeStock(2);
        CHECK_EQUAL(5, s.getAvailableQuantity());
        
        s.release(1);
        CHECK_EQUAL(6, s.getAvailableQuantity());
    }

    TEST(Stock_Throws)
    {
        CHECK_THROW(Stock(nullptr, -1), InvalidQuantityException);
        
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Book book(1, "It", 10, 500, "123", author, pub, genre);
        Stock s(&book, 10);

        CHECK_THROW(s.takeStock(15), OutOfStockException);
        CHECK_THROW(s.reserve(15), ReservationException);
        CHECK_THROW(s.release(1), ReservationException);
    }
}

SUITE(ReservationSuite)
{
    TEST(Reservation_Lifecycle)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Customer c(1, "Ivan", addr);
        Reservation r(1, &c);
        
        CHECK(r.isActiveReservation());
        r.extend(5);
        CHECK(!r.isExpired(10));
        
        r.cancelReservation();
        CHECK(!r.isActiveReservation());
    }

    TEST(Reservation_Throws)
    {
        CHECK_THROW(Reservation(1, nullptr), ReservationException);
        
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Customer c(1, "Ivan", addr);
        Reservation r(1, &c);
        r.cancelReservation();
        CHECK_THROW(r.cancelReservation(), ReservationException);
        CHECK_THROW(r.extend(5), ReservationException);
    }
}

SUITE(ShipmentSuite)
{
    TEST(Shipment_Lifecycle)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Parcel p1(1.5, "books", "T1", addr);
        Parcel p2(2.0, "books", "T2", addr);

        Shipment s(1);
        s.addParcel(p1);
        s.addParcel(p2);
        CHECK_EQUAL(2, s.getParcelsCount());
        CHECK_CLOSE(3.5, s.totalWeight(), 0.001);

        s.removeParcel(p2);
        CHECK_EQUAL(1, s.getParcelsCount());

        s.sendShipment();
        CHECK(s.isShipmentSent());
    }

    TEST(Shipment_Throws)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Parcel p1(1.5, "books", "T1", addr);
        
        Shipment s(1);
        CHECK_THROW(s.sendShipment(), InvalidQuantityException);
        
        s.addParcel(p1);
        s.sendShipment();
        CHECK_THROW(s.addParcel(p1), ShipmentAlreadySentException);
        CHECK_THROW(s.removeParcel(p1), ShipmentAlreadySentException);
        CHECK_THROW(s.sendShipment(), ShipmentAlreadySentException);
    }
}

SUITE(InventoryCheckSuite)
{
    TEST(InventoryCheck_Difference)
    {
        InventoryCheck ic(1, 10, 8);
        CHECK_EQUAL(-2, ic.getDifference());
        CHECK(!ic.isInventoryMatching());
        
        ic.fixDifference("date");
        CHECK_EQUAL(0, ic.getDifference());
        CHECK(ic.isInventoryMatching());
    }
}

SUITE(LoggerSuite)
{
    TEST(Logger_LogAndClear)
    {
        Logger l("test.log");
        CHECK(!l.isLogVerbose());
        l.logEntry();
        CHECK(!l.shouldLog(-1));
        l.clear();
    }
}

SUITE(BarcodeSuite)
{
    TEST(Barcode_Increment)
    {
        Barcode b("123");
        CHECK_EQUAL(0, b.getScanCount());
        b.incrementScans();
        CHECK_EQUAL(1, b.getScanCount());
    }
}

SUITE(ScannerSuite)
{
    TEST(Scanner_ScanAndCharge)
    {
        Scanner s(1);
        Barcode b("123");
        
        s.charge();
        s.turnScannerOn();
        s.scanBarcode(b);
        CHECK_EQUAL(1, b.getScanCount());
        
        s.turnScannerOff();
    }

    TEST(Scanner_Throws)
    {
        Scanner s(1);
        Barcode b("123");
        CHECK_THROW(s.scanBarcode(b), ScannerOfflineException);
    }
}

SUITE(RatingSuite)
{
    TEST(Rating_Average)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Book book(1, "It", 10, 500, "123", author, pub, genre);

        Rating r(&book);
        CHECK_CLOSE(0.0, r.getAverageRating(), 0.001);
        
        r.addVote(5);
        r.addVote(3);
        CHECK_CLOSE(4.0, r.getAverageRating(), 0.001);
        
        r.reset();
        CHECK_CLOSE(0.0, r.getAverageRating(), 0.001);
    }

    TEST(Rating_Throws)
    {
        CHECK_THROW(Rating(nullptr), InvalidRatingException);
        
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Author author(1, "King", 1947);
        Publisher pub(1, "AST", addr);
        Genre genre(1, "Horror", 18);
        Book book(1, "It", 10, 500, "123", author, pub, genre);

        Rating r(&book);
        CHECK_THROW(r.addVote(6), InvalidRatingException);
    }
}

SUITE(PersonSuite)
{
    TEST(Person_Phone)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Person p(1, "Ivan", addr);
        
        CHECK_EQUAL("Ivan", p.getName());
        CHECK_EQUAL("Russia, Moscow, Tverskaya 123456", p.getAddress());
        CHECK_EQUAL("person", p.role());
        
        p.setPhone("+79000000000");
        CHECK(p.isValidPhone());
        
        p.setPhone("8900");
        CHECK(!p.isValidPhone());
    }
}

SUITE(EmployeeSuite)
{
    TEST(Employee_Salary)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Employee e(1, "Ivan", addr, 1000.0);
        
        CHECK_CLOSE(1000.0, e.getSalary(), 0.001);
        e.raiseSalary(10);
        CHECK_CLOSE(1100.0, e.getSalary(), 0.001);
        CHECK_CLOSE(13200.0, e.annualSalary(), 0.001);
    }

    TEST(Employee_Throws)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        CHECK_THROW(Employee(1, "Ivan", addr, -100), SalaryException);
        Employee e(1, "Ivan", addr, 1000.0);
        CHECK_THROW(e.raiseSalary(-10), SalaryException);
    }
}

SUITE(CustomerSuite)
{
    TEST(Customer_PointsAndPurchase)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Customer c(1, "Ivan", addr);
        
        c.addPoints(100);
        c.spendPoints(50);
        c.registerPurchase(500.0);
    }

    TEST(Customer_Throws)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Customer c(1, "Ivan", addr);
        CHECK_THROW(c.addPoints(-10), InvalidQuantityException);
        CHECK_THROW(c.spendPoints(10), WarehouseException);
        CHECK_THROW(c.registerPurchase(-10), InvalidPriceException);
    }
}

SUITE(SupplierSuite)
{
    TEST(Supplier_Rating)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Supplier s(1, "Ivan", addr, "Company");
        
        s.rate(5);
        CHECK(s.isReliable());
        s.rate(3);
        CHECK(!s.isReliable());
        
        CHECK_THROW(s.rate(0), InvalidRatingException);
    }
}

SUITE(ManagerSuite)
{
    TEST(Manager_TeamAndBonus)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Manager m(1, "Ivan", addr, 1000.0, 5);
        
        CHECK_EQUAL(5, m.getTeamSize());
        CHECK_CLOSE(250.0, m.bonus(), 0.001);
        
        Employee e(2, "Petr", addr, 500.0);
        m.addToTeam(e);
        CHECK_EQUAL(6, m.getTeamSize());
    }

    TEST(Manager_Throws)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        CHECK_THROW(Manager(1, "Ivan", addr, 1000.0, -1), InvalidQuantityException);
        Manager m(1, "Ivan", addr, 1000.0, 5);
        CHECK_THROW(m.addToTeam(m), WarehouseException);
    }
}

SUITE(LoaderSuite)
{
    TEST(Loader_Lift)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Loader l(1, "Ivan", addr, 1000.0, 50.0);
        
        CHECK(l.canLift(40.0));
        CHECK(!l.canLift(60.0));
        
        Pallet p(1, 100.0);
        p.load(40.0);
        CHECK(l.canLiftPallet(p));
        CHECK(l.canWorkOvertime());
    }
}

SUITE(DriverSuite)
{
    TEST(Driver_Deliver)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Driver d(1, "Ivan", addr, 1000.0, "ABC");
        
        CHECK(d.hasLicense());
        d.addDistance(100.0);
        
        Shipment s(1);
        Parcel p(1.0, "books", "T1", addr);
        s.addParcel(p);
        
        d.deliver(s);
        CHECK(s.isShipmentSent());
    }

    TEST(Driver_Throws)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Driver d(1, "Ivan", addr, 1000.0, "");
        Shipment s(1);
        
        CHECK_THROW(d.deliver(s), WarehouseException);
        CHECK_THROW(d.addDistance(-10), InvalidQuantityException);
    }
}

SUITE(ReviewSuite)
{
    TEST(Review_PositivityAndLikes)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Customer c(1, "Ivan", addr);
        Author a(1, "King", 1947);
        Publisher p(1, "AST", addr);
        Genre g(1, "Horror", 18);
        Book b(1, "It", 10, 500, "123", a, p, g);

        Review r(&b, &c, 5, "Good");
        CHECK(r.isPositive());
        CHECK_EQUAL(5, r.getRating());
        CHECK_EQUAL("Good", r.getComment());
        r.like();
    }

    TEST(Review_Throws)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Customer c(1, "Ivan", addr);
        Author a(1, "King", 1947);
        Publisher p(1, "AST", addr);
        Genre g(1, "Horror", 18);
        Book b(1, "It", 10, 500, "123", a, p, g);

        CHECK_THROW(Review(&b, &c, 6, "Bad"), InvalidRatingException);
    }
}

SUITE(EmailNotificationSuite)
{
    TEST(EmailNotification_Send)
    {
        EmailNotification en("test@test.com", "Subject", "Msg");
        en.send();
    }

    TEST(EmailNotification_Throws)
    {
        EmailNotification en("invalid", "Subject", "Msg");
        CHECK_THROW(en.send(), WarehouseException);
    }
}

SUITE(SMSNotificationSuite)
{
    TEST(SMSNotification_Send)
    {
        SMSNotification sn("+79000000000", "Msg");
        sn.send();
    }

    TEST(SMSNotification_Throws)
    {
        SMSNotification sn("invalid", "Msg");
        CHECK_THROW(sn.send(), WarehouseException);
    }
}

SUITE(ShelfSuite)
{
    TEST(Shelf_Space)
    {
        Shelf s(1, 10);
        CHECK(s.isEmpty());
        s.put(5);
        CHECK_EQUAL(5, s.getFreeSpace());
        s.remove(2);
        CHECK_EQUAL(7, s.getFreeSpace());
        s.clearShelf();
        CHECK(s.isEmpty());
    }

    TEST(Shelf_Throws)
    {
        CHECK_THROW(Shelf(1, 0), InvalidQuantityException);
        
        Shelf s(1, 10);
        CHECK_THROW(s.put(11), ShelfOverflowException);
        CHECK_THROW(s.remove(1), InvalidQuantityException);
    }
}

SUITE(ContainerSuite)
{
    TEST(Container_Load)
    {
        Container c(100, 50.0);
        CHECK_EQUAL(100, c.volumeLimit());
        CHECK(c.canCarry(40.0));
        c.addLoad(40.0);
    }

    TEST(Container_Throws)
    {
        Container c(100, 50.0);
        CHECK_THROW(c.addLoad(60.0), ShelfOverflowException);
    }
}

SUITE(RackSuite)
{
    TEST(Rack_Placement)
    {
        Rack r(5, 100, 20.0);
        CHECK_EQUAL(5, r.getLevelsCount());
        CHECK_CLOSE(100.0, r.getTotalMaxLoad(), 0.001);
        CHECK(r.canPlaceOnLevel(1, 15.0));
        CHECK(r.fits(90.0));
        
        Shelf s(1, 10);
        r.addShelf(&s);
    }

    TEST(Rack_Throws)
    {
        CHECK_THROW(Rack(0, 100, 20.0), InvalidQuantityException);
        CHECK_THROW(Rack(5, 100, -20.0), InvalidDimensionsException);
    }
}

SUITE(PalletSuite)
{
    TEST(Pallet_Load)
    {
        Pallet p(1, 100.0);
        p.load(50.0);
        CHECK_CLOSE(50.0, p.getLoad(), 0.001);
        CHECK(!p.isFull());
        p.unload(20.0);
        CHECK_CLOSE(30.0, p.getLoad(), 0.001);
    }

    TEST(Pallet_Throws)
    {
        Pallet p(1, 100.0);
        CHECK_THROW(p.load(150.0), ShelfOverflowException);
        CHECK_THROW(p.unload(10.0), InvalidQuantityException);
    }
}

SUITE(AisleSuite)
{
    TEST(Aisle_RacksAndBlock)
    {
        Aisle a(1, 50.0, 10);
        CHECK_EQUAL(20, a.totalShelves());
        
        Rack r(5, 100, 20.0);
        a.addRack(&r);
        
        a.block();
        CHECK_THROW(a.addRack(&r), WarehouseException);
        a.unblock();
        a.addRack(&r);
    }
}

SUITE(SectionSuite)
{
    TEST(Section_AcceptsAndSpace)
    {
        Genre g(1, "Fantasy", 0);
        Section s(1, "Fantasy Zone", &g);
        
        CHECK(s.accepts(g));
        CHECK(s.hasSpace(10));
    }
}

SUITE(ClimateRoomSuite)
{
    TEST(ClimateRoom_Safety)
    {
        ClimateRoom c(1, 18.0, 45.0);
        CHECK(c.isSafeForRareBooks());
        
        ClimateRoom c2(1, 30.0, 60.0);
        CHECK(!c2.isSafeForRareBooks());
        c2.adjustTemperature();
    }
}

SUITE(WarehouseSuite)
{
    TEST(Warehouse_Sections)
    {
        Address addr("Russia", "Moscow", "Tverskaya", "123456");
        Warehouse w(1, addr);
        
        CHECK_EQUAL("Russia, Moscow, Tverskaya 123456", w.fullAddress());
        
        Genre g(1, "Fantasy", 0);
        Section s1(1, "Fantasy Zone", &g);
        
        w.addSection(&s1);
        CHECK_EQUAL(1, w.getSectionsCount());
        
        Section* found = w.findSection(g);
        CHECK(found == &s1);
    }
}