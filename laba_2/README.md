# Лабораторная работа №2. Крупный ООП-проект

Предметная область: **книжный склад**

Формат строки: `Класс | число полей | число методов | связанные классы`.

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| Address | 4 | 2 | — |
| Aisle | 5 | 4 | Rack |
| Atlas | 1 | 2 | Book |
| AudioBook | 2 | 2 | StorageItem |
| Author | 4 | 4 | — |
| Barcode | 2 | 2 | — |
| Book | 6 | 6 | StorageItem, Author, Publisher, Genre |
| CardPayment | 2 | 2 | Payment |
| Cart | 3 | 6 | Customer, StorageItem, PromoCode |
| CashPayment | 1 | 2 | Payment |
| ClimateRoom | 4 | 2 | — |
| Comic | 1 | 2 | Book |
| Container | 3 | 3 | — |
| Customer | 2 | 4 | Person |
| Dimensions | 4 | 6 | — |
| Document | 3 | 3 | — |
| Driver | 2 | 4 | Employee, Shipment |
| EmailNotification | 1 | 2 | Notification |
| Employee | 2 | 4 | Person |
| Genre | 3 | 3 | — |
| Hardcover | 1 | 2 | Book |
| InventoryCheck | 4 | 3 | — |
| Invoice | 2 | 4 | Document |
| Loader | 2 | 4 | Employee, Pallet |
| Logger | 3 | 5 | Document |
| Magazine | 2 | 2 | StorageItem |
| Manager | 1 | 4 | Employee |
| Notification | 2 | 1 | — |
| Order | 3 | 3 | Document, Customer |
| OrderItem | 3 | 2 | StorageItem, Price |
| Pallet | 3 | 4 | — |
| Paperback | 1 | 2 | Book |
| Parcel | 5 | 4 | Address |
| Payment | 3 | 2 | — |
| Person | 4 | 5 | Address |
| Price | 2 | 4 | — |
| PromoCode | 4 | 4 | — |
| Publisher | 4 | 3 | Address |
| PurchaseOrder | 1 | 2 | Order, Supplier |
| Rack | 3 | 5 | Container, Shelf |
| Rating | 3 | 4 | Book, Review |
| Receipt | 1 | 2 | Document |
| Reservation | 4 | 4 | Customer |
| ReturnNote | 2 | 3 | Document |
| Review | 5 | 4 | Book, Customer |
| Scanner | 4 | 4 | Barcode |
| Section | 4 | 2 | Genre |
| Shelf | 3 | 6 | StorageItem |
| Shipment | 3 | 6 | Parcel |
| SMSNotification | 1 | 2 | Notification |
| Stock | 3 | 4 | StorageItem |
| StorageItem | 4 | 6 | — |
| Supplier | 2 | 3 | Person |
| Warehouse | 4 | 5 | Address, Section, Genre |

### Исключения (15)

- WarehouseException
- InvalidPriceException
- InvalidDiscountException
- InvalidQuantityException
- OutOfStockException
- ReservationException
- SalaryException
- ShelfOverflowException
- PaymentFailedException
- AgeRestrictionException
- ScannerOfflineException
- InvalidDimensionsException
- InvalidRatingException
- ShipmentAlreadySentException
- InvalidPromoCodeException

### Итоговая статистика

| Показатель | Значение |
|---|---|
| Классы | 54 |
| Поля | 151 |
| Поведения | 116 |
| Ассоциации | 54 |
| Исключения | 15 |
