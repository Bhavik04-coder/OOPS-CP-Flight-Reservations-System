# Airline Reservation System - Pro Level Edition

## Overview
A comprehensive C++ console-based Airline Reservation System that provides complete flight booking and management capabilities. This system supports multiple user roles (Passengers and Administrators) with extensive features for flight management, booking, and reporting.

## Features

### 🎫 Passenger Features
- **User Registration & Login** - Secure account creation and authentication
- **Flight Search** - Search flights by origin, destination, and date
- **Flight Booking** - Interactive seat selection with visual seat maps
- **Booking Management** - View and cancel existing bookings
- **Profile Management** - Personal information management

### 👨‍💼 Admin Features
- **Flight Management** - Add and remove flights from the system
- **Booking Oversight** - View all passenger bookings
- **Passenger Management** - Access to all registered passenger data
- **Occupancy Reports** - Detailed flight occupancy statistics
- **System Analytics** - Comprehensive revenue and booking reports
- **Admin Registration** - Super admin can register new administrators

### ✈️ Flight Management
- **Multiple Airlines** - Support for various airline carriers
- **Seat Mapping** - Visual seat availability display
- **Real-time Availability** - Live seat booking and cancellation
- **Route Management** - Multiple origin-destination combinations

## Technical Specifications

### System Requirements
- **Compiler**: C++11 compatible compiler (GCC, Clang, MSVC)
- **Platform**: Windows, Linux, macOS
- **Storage**: File-based persistence (no database required)
- **Memory**: Minimal system requirements

### Data Persistence
The system uses text files for data storage:
- `passengers.txt` - Passenger accounts and profiles
- `admins.txt` - Administrator accounts
- `flights.txt` - Flight schedules and seat maps
- `bookings.txt` - Booking records and transactions

### Class Structure
- **User** (Base class)
  - **Passenger** - Manages passenger data and bookings
  - **Admin** - Handles administrative privileges
- **Flight** - Flight details and seat management
- **Booking** - Booking transactions and status
- **ReservationSystem** - Main system controller

## Installation & Compilation

### Compilation Instructions
```bash
# Using g++
g++ -std=c++11 -o airline_reservation main.cpp

# Using clang++
clang++ -std=c++11 -o airline_reservation main.cpp

# Using Microsoft Visual C++
cl /EHsc main.cpp
```

### Running the Application
```bash
./airline_reservation
```

## Default Credentials

### Sample Admin Account
- **User ID**: `admin`
- **Password**: `admin123`
- **Level**: SUPER admin

### Sample Flights
The system includes pre-loaded flights:
- AI101: New Delhi → Mumbai
- AI102: Mumbai → Bangalore  
- SG201: Bangalore → Chennai
- IG301: Chennai → Kolkata
- AI103: Delhi → Goa

## Usage Guide

### For Passengers
1. **Register** a new account with personal details
2. **Login** with your credentials
3. **Search** for available flights
4. **Select** preferred seat from interactive map
5. **Confirm** booking and receive booking ID
6. **Manage** existing bookings from dashboard

### For Administrators
1. **Login** with admin credentials
2. **Add/Remove** flights as needed
3. **Monitor** all system bookings
4. **Generate** occupancy and revenue reports
5. **View** passenger statistics and trends

## File Structure
```
airline_reservation/
├── main.cpp                 # Main application file
├── passengers.txt          # Passenger data storage
├── admins.txt             # Administrator data storage  
├── flights.txt            # Flight schedules and seat data
├── bookings.txt           # Booking records
└── README.md             # This file
```

## Key Features in Detail

### 🔐 Authentication System
- Secure password-based authentication
- Role-based access control
- Session management

### 💺 Seat Management  
- Visual seat mapping (A1, B2, C3, etc.)
- Real-time seat availability
- Automatic seat allocation
- Cancellation with seat release

### 📊 Reporting System
- Financial revenue tracking
- Occupancy percentage calculations
- Popular route analytics
- Booking trend analysis

### 💾 Data Persistence
- Automatic save/load on startup/shutdown
- Transaction integrity
- Data consistency checks

## Error Handling
- Input validation for all user entries
- Email and phone number format verification
- Duplicate registration prevention
- Graceful error recovery

## Limitations & Future Enhancements

### Current Limitations
- Console-based interface only
- Single-threaded operations
- Basic file-based storage
- No network connectivity

### Potential Enhancements
- Web-based GUI interface
- Database integration
- Payment gateway integration
- Email notifications
- Mobile application
- Real-time flight status updates

## Development Notes

### Code Organization
- Object-oriented design with inheritance
- Separation of concerns
- Modular class structure
- Comprehensive error handling

### Data Validation
- Email format verification
- Phone number validation
- Date format checking
- Input sanitization

## Support
For issues or questions regarding this system, please ensure:
1. All data files have proper read/write permissions
2. Compiler supports C++11 standards
3. Sufficient disk space for data storage

## License
This Airline Reservation System is provided as-is for educational and demonstration purposes.

---

**Note**: This is a console-based application. All interactions occur through the command line interface. Ensure your terminal supports ANSI escape codes for optimal display of seat maps and menus.
