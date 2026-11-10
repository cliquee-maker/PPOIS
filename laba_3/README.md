# Лабораторная работа №2. Крупный ООП-проект

Предметная область: **система бронирования путешествий**


| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| AgencyContract | 1 | 2 | BookingConfirmation, HotelPartner |
| AuditLog | 2 | 3 | — |
| Availability | 3 | 3 | TravelService, DateRange |
| BaggageAllowance | 3 | 5 | — |
| BoardingPass | 5 | 4 | Location |
| BookingCalendar | 2 | 3 | DateRange |
| BookingConfirmation | 3 | 5 | TravelDocument, Tourist |
| BookingRequest | 4 | 3 | Tourist, TravelService, DateRange |
| BusinessFlight | 1 | 3 | FlightBooking |
| Cart | 3 | 6 | Tourist, CartItem, TravelService, Price |
| CartItem | 3 | 3 | TravelService, Price |
| ContactInfo | 3 | 5 | — |
| CruiseBooking | 2 | 4 | TravelService |
| Currency | 3 | 4 | — |
| DateRange | 2 | 6 | — |
| DestinationRoute | 5 | 6 | FlightDeck |
| EconomyFlight | 1 | 3 | FlightBooking |
| Employee | 2 | 6 | Person |
| ETicket | 2 | 3 | TravelDocument |
| ExcursionBooking | 2 | 3 | TravelService |
| FlightBooking | 6 | 7 | TravelService, TravelAgent, TourOperator, TourCategory |
| FlightDeck | 3 | 6 | TransportCapacity, RoomBlock |
| Guide | 2 | 4 | Employee |
| HotelBooking | 2 | 4 | TravelService |
| HotelPartner | 2 | 4 | Person |
| InsurancePolicy | 3 | 5 | TravelDocument |
| InsuranceService | 2 | 4 | TravelService |
| Invoice | 3 | 7 | TravelDocument |
| Itinerary | 1 | 4 | Reservation |
| Location | 3 | 5 | — |
| LuggageCompartment | 3 | 5 | — |
| Payment | 4 | 4 | Reservation, PaymentCard, Price |
| PaymentCard | 3 | 3 | — |
| Person | 4 | 6 | Location, ContactInfo |
| Price | 2 | 8 | — |
| PromoCode | 4 | 4 | DateRange |
| Reservation | 6 | 7 | Tourist, TravelService, DateRange, Price |
| RoomBlock | 4 | 7 | TravelService |
| SearchQuery | 5 | 3 | Location, DateRange, TravelService |
| SupportAgent | 2 | 3 | Employee |
| TourCategory | 3 | 5 | — |
| Tourist | 3 | 7 | Person |
| TourManager | 1 | 5 | Employee |
| TourOperator | 4 | 5 | Location |
| TourSection | 4 | 5 | TourCategory |
| Transaction | 4 | 4 | Price |
| Transfer | 5 | 5 | TravelService, TransferDriver, Location |
| TransferDriver | 2 | 5 | Employee, Transfer |
| TransportCapacity | 3 | 4 | — |
| TravelAgencySystem | 4 | 6 | Location, TourSection, TourCategory |
| TravelAgent | 4 | 5 | TourCategory |
| TravelDocument | 3 | 5 | — |
| TravelPackage | 3 | 5 | TravelService |
| TravelService | 4 | 9 | — |
| TravelVoucher | 1 | 4 | TravelDocument |
| VIPLounge | 4 | 3 | — |
| VisaDocument | 2 | 3 | TravelDocument, DateRange |
| WishList | 2 | 4 | Tourist, TravelService, Cart, Price |

### Исключения (14)

- TravelException
- InvalidPriceException
- FullyBookedException
- ReservationException
- PaymentFailedException
- InvalidPromoCodeException
- InvalidPassportException
- TransportCancelledException
- ExcessBaggageException
- VisaRequiredException
- InvalidDateRangeException
- DuplicateBookingException
- RefundNotAllowedException
- UnauthorizedAccessException

### Итоговая статистика

| Показатель | Значение |
|---|---|
| Классы | 58 |
| Поля | 172 |
| Поведения | 180 |
| Ассоциации | 74 |
| Исключения | 14 |

