#include <UnitTest++/UnitTest++.h>

#include "Basic.hpp"
#include "Booking.hpp"
#include "Cart.hpp"
#include "Documents.hpp"
#include "Operation.hpp"
#include "People.hpp"
#include "Review.hpp"
#include "Service.hpp"
#include "System.hpp"
#include "except.hpp"

SUITE(BasicTests)
{
    TEST(Currency_Logic)
    {
        Currency usd("USD", "$", 1.0);
        CHECK_CLOSE(100.0, usd.convertFromUsd(100.0), 0.001);
        CHECK_EQUAL("$100.00", usd.format(100.0));
        CHECK_THROW(Currency("", "$", 1.0), InvalidPriceException);
    }

    TEST(Price_Logic)
    {
        Price p(100.0, "USD");
        CHECK_CLOSE(120.0, p.getPriceWithTax(20), 0.001);
        CHECK_CLOSE(80.0, p.getPriceWithDiscount(20), 0.001);
        CHECK_THROW(Price(-10, "USD"), InvalidPriceException);
        CHECK_THROW(p.add(Price(10, "EUR")), InvalidPriceException);
    }

    TEST(DateRange_Logic)
    {
        DateRange dr("2026-01-01", "2026-01-10");
        CHECK_EQUAL(9, dr.nights());
        CHECK(dr.contains("2026-01-05"));
        
        DateRange dr2("2026-01-05", "2026-01-15");
        CHECK(dr.overlaps(dr2));
        CHECK_THROW(DateRange("2026-01-10", "2026-01-01"), InvalidDateRangeException);
    }

    TEST(ContactInfo_Logic)
    {
        ContactInfo ci("12345678", "test@test.com");
        CHECK(ci.isValidPhone());
        CHECK(ci.isValidEmail());
        CHECK_EQUAL("123***78", ci.maskedPhone());
    }

    TEST(Location_Logic)
    {
        Location moscow("Russia", "Moscow", "SVO");
        Location spb("Russia", "SPb", "LED");
        CHECK(moscow.sameCountry(spb));
        CHECK_CLOSE(500.0, moscow.distanceTo(spb), 0.001);
    }

    TEST(BaggageAllowance_Logic)
    {
        BaggageAllowance ba(20.0, 2, 1.5);
        CHECK(ba.fits(20.0, 1.5));
        CHECK_CLOSE(5.0, ba.excessWeight(25.0), 0.001);
    }

    TEST(TravelAgentAndCategory)
    {
        TourCategory tc(1, "Adult", 18);
        CHECK(tc.allowedFor(18));
        CHECK(!tc.allowedFor(17));
        
        TravelAgent ta(1, "Ivan", 5);
        CHECK(ta.canHandle(tc));
    }
}

SUITE(PeopleTests)
{
    TEST(Person_And_Employee)
    {
        Location loc("Russia", "Moscow", "SVO");
        Person p(1, "John", loc);
        CHECK_EQUAL("John", p.getName());
        
        Employee e(1, "Bob", loc, 1000.0);
        e.raiseSalary(10);
        CHECK_CLOSE(1100.0, e.getSalary(), 0.001);
    }

    TEST(Tourist_Loyalty)
    {
        Location loc("Russia", "Moscow", "SVO");
        Tourist t(1, "Alice", loc);
        CHECK_EQUAL("Bronze", t.loyaltyTier());
        t.registerBooking(6000.0);
        CHECK_EQUAL("Gold", t.loyaltyTier());
    }

    TEST(HotelPartner_Logic)
    {
        Location loc("Russia", "Moscow", "SVO");
        HotelPartner hp(1, "Owner", loc, "Hilton");
        hp.rate(5);
        CHECK(hp.isVerified());
    }

    TEST(TransferDriver_Logic)
    {
        Location loc("Russia", "Moscow", "SVO");
        TransferDriver td(1, "Bob", loc, 1000.0, "Toyota");
        td.addDistance(100.0);
        CHECK_CLOSE(8.0, td.fuelCost(1.0), 0.001);
    }
}

SUITE(ServiceTests)
{
    TEST(HotelBooking_Logic)
    {
        HotelBooking hb(1, "Hilton", 10, 5, 5);
        CHECK_CLOSE(5.0, hb.durationDays(), 0.001);
        CHECK(hb.isLuxury());
        hb.bookSlots(10);
        CHECK(!hb.isAvailable());
    }

    TEST(FlightBooking_Logic)
    {
        Location loc("Russia", "Moscow", "SVO");
        TravelAgent ta(1, "Ivan", 5);
        TourOperator to(1, "Pegas", loc);
        TourCategory tc(1, "Adult", 18);
        
        FlightBooking fb(1, "Flight", 10, 101, "SU101", ta, to, tc);
        CHECK_CLOSE(1.0, fb.durationDays(), 0.001);
        CHECK(fb.isBookedVia(ta));
    }

    TEST(Transfer_Logic)
    {
        Location loc1("Russia", "Moscow", "SVO");
        Location loc2("Russia", "Moscow", "VKO");
        TransferDriver td(1, "Bob", loc1, 1000.0, "Toyota");
        
        Transfer tr(1, "Airport transfer", 4, &td, loc1, loc2, 60.0);
        CHECK(tr.isLongDistance());
        tr.complete();
        CHECK_THROW(tr.complete(), ReservationException);
    }

    TEST(TravelPackage_Composite)
    {
        TravelPackage tp("Weekend", 10.0);
        HotelBooking hb(1, "Hilton", 10, 2, 5);
        hb.setPrice(100.0);
        tp.addService(&hb);
        
        CHECK_CLOSE(90.0, tp.totalPrice(), 0.001);
        CHECK_EQUAL(2, tp.totalDuration());
    }
}

SUITE(BookingTests)
{
    TEST(Reservation_Lifecycle)
    {
        Location loc("Russia", "Moscow", "SVO");
        Tourist tourist(1, "Ivan", loc);
        DateRange dr("2026-01-01", "2026-01-05");
        Reservation res(1, &tourist, dr, Price(100.0, "USD"));
        
        CHECK_THROW(res.confirm(), ReservationException);
        HotelBooking hb(1, "Hilton", 10, 4, 5);
        res.addService(&hb);
        CHECK(res.confirm());
    }

    TEST(Itinerary_Logic)
    {
        Location loc("Russia", "Moscow", "SVO");
        Tourist tourist(1, "Ivan", loc);
        DateRange dr("2026-01-01", "2026-01-05");
        Reservation res(1, &tourist, dr, Price(100.0, "USD"));
        
        Itinerary it;
        it.addReservation(&res);
        CHECK_EQUAL(4, it.totalDays());
    }

    TEST(SearchQuery_Logic)
    {
        Location from("Russia", "Moscow", "SVO");
        Location to("France", "Paris", "CDG");
        DateRange dr("2026-01-01", "2026-01-10");
        SearchQuery sq(from, to, dr, 2, 1000.0);
        
        HotelBooking hb(1, "Hilton", 10, 5, 5);
        CHECK(sq.matches(hb));
        CHECK_EQUAL("SVO-CDG-2026-01-01-2", sq.cacheKey());
    }

    TEST(Availability_Logic)
    {
        HotelBooking hb(1, "Hilton", 10, 5, 5);
        DateRange dr("2026-01-01", "2026-01-10");
        Availability av(&hb, dr, 5);
        
        CHECK(av.isAvailable(3));
        av.hold(3);
        CHECK(!av.isAvailable(3));
        CHECK_THROW(av.hold(6), FullyBookedException);
    }
}

SUITE(CartTests)
{
    TEST(Cart_Operations)
    {
        Location loc("Russia", "Moscow", "SVO");
        Tourist tourist(1, "Ivan", loc);
        Cart cart(&tourist);
        HotelBooking hb(1, "Hilton", 10, 4, 5);
        
        cart.addItem(&hb, Price(100.0, "USD"));
        CHECK_CLOSE(100.0, cart.total(), 0.001);
        
        cart.applyPromo("SAVE10");
        CHECK_CLOSE(90.0, cart.total(), 0.001);
        
        cart.removeItem(hb);
        CHECK(cart.isEmpty());
    }
}

SUITE(DocumentsTests)
{
    TEST(Invoice_Calculations)
    {
        Invoice inv(1, "2026-01-01", 100.0);
        CHECK_CLOSE(110.0, inv.total(), 0.001);
        inv.markPaid();
        CHECK(inv.isPaid());
    }

    TEST(InsurancePolicy_Claims)
    {
        InsurancePolicy ip(1, "2026-01-01", "Medical", 100000.0);
        CHECK(ip.isPremium());
        ip.approveClaim(50000.0);
        CHECK(!ip.covers(60000.0));
    }

    TEST(BookingConfirmation_Status)
    {
        Location loc("Russia", "Moscow", "SVO");
        Tourist tourist(1, "Ivan", loc);
        BookingConfirmation bc(1, "2026-01-01", &tourist, 2, "pending");
        CHECK(bc.isActive());
        bc.cancel();
        CHECK(!bc.isActive());
    }

    TEST(VisaDocument_Logic)
    {
        DateRange valid("2026-01-01", "2026-12-31");
        VisaDocument vd(1, "2026-01-01", "France", valid);
        CHECK(!vd.isExpired("2026-06-01"));
        CHECK(vd.isExpired("2027-01-01"));
    }
}

SUITE(OperationTests)
{
    TEST(PromoCode_Usage)
    {
        DateRange dr("2026-01-01", "2026-12-31");
        PromoCode pc("SALE", 10.0, dr, 1);
        
        CHECK(pc.isValid("2026-06-01"));
        pc.consume();
        CHECK(!pc.isValid("2026-06-01"));
    }

    TEST(PaymentCard_Validation)
    {
        PaymentCard pc("4111-1111-1111-1111", "IVAN", "12/28");
        CHECK(pc.isValid());
        CHECK_EQUAL("**** **** **** 1111", pc.maskedNumber());
    }

    TEST(Payment_ProcessAndRefund)
    {
        Location loc("Russia", "Moscow", "SVO");
        Tourist tourist(1, "Ivan", loc);
        DateRange dr("2026-01-01", "2026-01-10");
        Reservation res(1, &tourist, dr, Price(100.0, "USD"));
        PaymentCard card("4111111111111111", "IVAN", "12/28");
        
        Payment p(&res, card, Price(100.0, "USD"));
        CHECK(p.process());
        CHECK(p.isSuccessful());
        p.refund(50.0);
        CHECK_THROW(p.refund(100.0), RefundNotAllowedException);
    }

    TEST(Transaction_Logic)
    {
        Transaction t(1, "payment", Price(100.0, "USD"), "2026-01-01");
        CHECK(t.isCredit());
        t.reverse();
        CHECK(!t.isCredit());
    }
}

SUITE(ReviewTests)
{
    TEST(BoardingPass_Lifecycle)
    {
        Location loc("Russia", "Moscow", "SVO");
        BoardingPass bp("12A", "Ivan", "TK123", loc);
        CHECK(!bp.isCheckedIn());
        bp.checkIn();
        CHECK(bp.isCheckedIn());
        CHECK_THROW(bp.checkIn(), ReservationException);
    }
}

SUITE(SystemTests)
{
    TEST(RoomBlock_Reserve)
    {
        RoomBlock rb(1, 10);
        rb.reserveRooms(5);
        CHECK_EQUAL(5, rb.getFreeCapacity());
        CHECK(rb.isFullyBooked() == false);
    }

    TEST(TransportCapacity_Weight)
    {
        TransportCapacity tc(10, 100.0);
        tc.addBaggage(50.0);
        CHECK_CLOSE(50.0, tc.remainingWeight(), 0.001);
        CHECK_THROW(tc.addBaggage(60.0), ExcessBaggageException);
    }

    TEST(FlightDeck_Logic)
    {
        FlightDeck fd(3, 100, 20.0);
        CHECK_EQUAL(3, fd.getClassesCount());
        CHECK_CLOSE(60.0, fd.getTotalMaxWeight(), 0.001);
    }

    TEST(BookingCalendar_Logic)
    {
        BookingCalendar bc(2);
        DateRange dr1("2026-01-01", "2026-01-05");
        DateRange dr2("2026-01-02", "2026-01-06");
        DateRange dr3("2026-01-03", "2026-01-07");
        
        bc.addBooking(dr1);
        bc.addBooking(dr2);
        CHECK(!bc.isFree(dr3));
    }
}