# AirlinePassengerReservationSystem-ver.C

## Features

1. Add new flight  
2. Search flights (by departure and destination)  
3. Book tickets (alternative flights or waitlist if tickets are insufficient)  
4. Refund tickets (releases seats and serves waitlist in FIFO order)  
5. Display all flights  
6. Delete flight (with confirmation)  
0. Exit and save

Data is saved to `flights.dat` and loaded automatically on startup.

---

## Build & Run

Compile `APRS-CSourceCode.c` with Visual Studio or MinGW on Windows.

---

## Files

- `APRS-CSourceCode.c` — complete source code  
- `APPS-MiniProjectReport.docx` — project report

---

## Data Structures

- Flights stored as a singly linked list  
- Each flight maintains two passenger lists: booked and waiting

---

## Author

Jelliny  
January 2026


