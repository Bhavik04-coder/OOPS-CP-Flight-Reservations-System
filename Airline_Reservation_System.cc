#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cctype>

using namespace std;

 
string generateId(const string& prefix) {
    static int counter = 1000;
    return prefix + to_string(counter++);
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    return to_string(ltm->tm_mday) + "/" + 
           to_string(1 + ltm->tm_mon) + "/" + 
           to_string(1900 + ltm->tm_year);
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string toUpper(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool isValidEmail(const string& email) {
    return email.find('@') != string::npos && email.find('.') != string::npos;
}

bool isValidPhone(const string& phone) {
    return phone.length() >= 10 && all_of(phone.begin(), phone.end(), ::isdigit);
}


class User {
protected:
    string userId;
    string password;
    string name;
    string email;
    string userType;

public:
    User() : userId(""), password(""), name(""), email(""), userType("") {}
    
    User(string id, string pass, string n, string mail, string type)
        : userId(id), password(pass), name(n), email(mail), userType(type) {}
    
    virtual ~User() {}
    
    virtual void displayInfo() const {
        cout << "User ID: " << userId << endl;
        cout << "Name: " << name << endl;
        cout << "Email: " << email << endl;
        cout << "Role: " << userType << endl;
    }
    
    virtual string getRole() const = 0;
    
    string getUserId() const { return userId; }
    string getPassword() const { return password; }
    string getName() const { return name; }
    string getEmail() const { return email; }
    string getUserType() const { return userType; }
    
    void setPassword(string pass) { password = pass; }
    void setName(string n) { name = n; }
    void setEmail(string mail) { email = mail; }
};

 
class Passenger : public User {
private:
    string phone;
    string passportNumber;
    vector<string> bookingIds;

public:
    Passenger() : User(), phone(""), passportNumber("") {}
    
    Passenger(string id, string pass, string n, string mail, string ph, string passport)
        : User(id, pass, n, mail, "PASSENGER"), phone(ph), passportNumber(passport) {}
    
    void displayInfo() const override {
        User::displayInfo();
        cout << "Phone: " << phone << endl;
        cout << "Passport: " << passportNumber << endl;
        cout << "Total Bookings: " << bookingIds.size() << endl;
    }
    
    string getRole() const override { return "PASSENGER"; }
    
    string getPhone() const { return phone; }
    string getPassportNumber() const { return passportNumber; }
    vector<string> getBookingIds() const { return bookingIds; }
    
    void addBooking(string bookingId) {
        bookingIds.push_back(bookingId);
    }
    
    void removeBooking(string bookingId) {
        auto it = find(bookingIds.begin(), bookingIds.end(), bookingId);
        if (it != bookingIds.end()) {
            bookingIds.erase(it);
        }
    }
    
    string toFileString() const {
        string bookings = "";
        for (size_t i = 0; i < bookingIds.size(); i++) {
            bookings += bookingIds[i];
            if (i < bookingIds.size() - 1) bookings += ",";
        }
        return userId + "|" + password + "|" + name + "|" + email + "|" + 
               phone + "|" + passportNumber + "|" + bookings;
    }
    
    static Passenger fromFileString(const string& line) {
        size_t pos = 0, prev = 0;
        vector<string> tokens;
        
        while ((pos = line.find('|', prev)) != string::npos) {
            tokens.push_back(line.substr(prev, pos - prev));
            prev = pos + 1;
        }
        tokens.push_back(line.substr(prev));
        
        if (tokens.size() < 6) return Passenger();
        
        Passenger p(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
        
        if (tokens.size() > 6 && !tokens[6].empty()) {
            size_t bPos = 0, bPrev = 0;
            while ((bPos = tokens[6].find(',', bPrev)) != string::npos) {
                p.addBooking(tokens[6].substr(bPrev, bPos - bPrev));
                bPrev = bPos + 1;
            }
            if (bPrev < tokens[6].length()) {
                p.addBooking(tokens[6].substr(bPrev));
            }
        }
        
        return p;
    }
};


class Admin : public User {
private:
    string adminLevel;

public:
    Admin() : User(), adminLevel("") {}
    
    Admin(string id, string pass, string n, string mail, string level)
        : User(id, pass, n, mail, "ADMIN"), adminLevel(level) {}
    
    void displayInfo() const override {
        User::displayInfo();
        cout << "Admin Level: " << adminLevel << endl;
    }
    
    string getRole() const override { return "ADMIN"; }
    string getAdminLevel() const { return adminLevel; }
    
    string toFileString() const {
        return userId + "|" + password + "|" + name + "|" + email + "|" + adminLevel;
    }
    
    static Admin fromFileString(const string& line) {
        size_t pos = 0, prev = 0;
        vector<string> tokens;
        
        while ((pos = line.find('|', prev)) != string::npos) {
            tokens.push_back(line.substr(prev, pos - prev));
            prev = pos + 1;
        }
        tokens.push_back(line.substr(prev));
        
        if (tokens.size() < 5) return Admin();
        
        return Admin(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4]);
    }
};

 
class Flight {
private:
    string flightNumber;
    string airline;
    string origin;
    string destination;
    string departureDate;
    string departureTime;
    string arrivalTime;
    int totalSeats;
    int availableSeats;
    double baseFare;
    map<string, bool> seatMap;

public:
    Flight() : totalSeats(0), availableSeats(0), baseFare(0.0) {}
    
    Flight(string num, string air, string org, string dest, string date,
           string depTime, string arrTime, int seats, double fare)
        : flightNumber(num), airline(air), origin(org), destination(dest),
          departureDate(date), departureTime(depTime), arrivalTime(arrTime),
          totalSeats(seats), availableSeats(seats), baseFare(fare) {
        
        initializeSeats();
    }
    
    void initializeSeats() {
        char rows[] = {'A', 'B', 'C', 'D', 'E', 'F'};
        int numRows = (totalSeats + 5) / 6;
        
        for (int i = 1; i <= numRows; i++) {
            for (char c : rows) {
                if (seatMap.size() < (size_t)totalSeats) {
                    string seat = string(1, c) + to_string(i);
                    seatMap[seat] = true;
                }
            }
        }
    }
    
    bool bookSeat(string seatNum) {
        if (seatMap.find(seatNum) != seatMap.end() && seatMap[seatNum]) {
            seatMap[seatNum] = false;
            availableSeats--;
            return true;
        }
        return false;
    }
    
    bool cancelSeat(string seatNum) {
        if (seatMap.find(seatNum) != seatMap.end() && !seatMap[seatNum]) {
            seatMap[seatNum] = true;
            availableSeats++;
            return true;
        }
        return false;
    }
    
    bool isSeatAvailable(string seatNum) const {
        auto it = seatMap.find(seatNum);
        return it != seatMap.end() && it->second;
    }
    
    vector<string> getAvailableSeats() const {
        vector<string> available;
        for (const auto& seat : seatMap) {
            if (seat.second) {
                available.push_back(seat.first);
            }
        }
        return available;
    }
    
    void displayFlightInfo() const {
        cout << "\n========================================" << endl;
        cout << "Flight: " << flightNumber << " (" << airline << ")" << endl;
        cout << "Route: " << origin << " -> " << destination << endl;
        cout << "Date: " << departureDate << endl;
        cout << "Departure: " << departureTime << " | Arrival: " << arrivalTime << endl;
        cout << "Available Seats: " << availableSeats << "/" << totalSeats << endl;
        cout << "Base Fare: ₹" << fixed << setprecision(2) << baseFare << endl;
        cout << "========================================" << endl;
    }
    
    void displaySeatMap() const {
        cout << "\n=== SEAT MAP (O = Available, X = Booked) ===" << endl;
        cout << "     A  B  C    D  E  F" << endl;
        
        int numRows = (totalSeats + 5) / 6;
        for (int i = 1; i <= numRows; i++) {
            cout << setw(2) << i << "  ";
            char cols[] = {'A', 'B', 'C', 'D', 'E', 'F'};
            for (int j = 0; j < 6; j++) {
                string seat = string(1, cols[j]) + to_string(i);
                if (seatMap.find(seat) != seatMap.end()) {
                    cout << " " << (seatMap.at(seat) ? "O" : "X") << " ";
                } else {
                    cout << "   ";
                }
                if (j == 2) cout << "  ";
            }
            cout << endl;
        }
        cout << "=============================================" << endl;
    }
    
    string getFlightNumber() const { return flightNumber; }
    string getAirline() const { return airline; }
    string getOrigin() const { return origin; }
    string getDestination() const { return destination; }
    string getDepartureDate() const { return departureDate; }
    string getDepartureTime() const { return departureTime; }
    string getArrivalTime() const { return arrivalTime; }
    int getAvailableSeatsCount() const { return availableSeats; }
    int getTotalSeats() const { return totalSeats; }
    double getBaseFare() const { return baseFare; }
    
    void setAvailableSeats(int seats) { availableSeats = seats; }
    
    string toFileString() const {
        string bookedSeats = "";
        for (auto& seat : seatMap) {
            if (!seat.second) {
                bookedSeats += seat.first + ",";
            }
        }
        if (!bookedSeats.empty()) bookedSeats.pop_back();
        
        return flightNumber + "|" + airline + "|" + origin + "|" + destination + "|" +
               departureDate + "|" + departureTime + "|" + arrivalTime + "|" +
               to_string(totalSeats) + "|" + to_string(availableSeats) + "|" +
               to_string(baseFare) + "|" + bookedSeats;
    }
    
    static Flight fromFileString(const string& line) {
        size_t pos = 0, prev = 0;
        vector<string> tokens;
        
        while ((pos = line.find('|', prev)) != string::npos) {
            tokens.push_back(line.substr(prev, pos - prev));
            prev = pos + 1;
        }
        tokens.push_back(line.substr(prev));
        
        if (tokens.size() < 10) return Flight();
        
        Flight f(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                 tokens[5], tokens[6], stoi(tokens[7]), stod(tokens[9]));
        
        f.setAvailableSeats(stoi(tokens[8]));
        
        if (tokens.size() > 10 && !tokens[10].empty()) {
            size_t sPos = 0, sPrev = 0;
            while ((sPos = tokens[10].find(',', sPrev)) != string::npos) {
                f.bookSeat(tokens[10].substr(sPrev, sPos - sPrev));
                sPrev = sPos + 1;
            }
            if (sPrev < tokens[10].length()) {
                f.bookSeat(tokens[10].substr(sPrev));
            }
        }
        
        return f;
    }
};

 
class Booking {
private:
    string bookingId;
    string passengerId;
    string flightNumber;
    string seatNumber;
    string bookingDate;
    double totalFare;
    string status;

public:
    Booking() : totalFare(0.0), status("PENDING") {}
    
    Booking(string bId, string pId, string fNum, string seat, double fare)
        : bookingId(bId), passengerId(pId), flightNumber(fNum),
          seatNumber(seat), totalFare(fare), status("CONFIRMED") {
        bookingDate = getCurrentDate();
    }
    
    void displayBookingInfo() const {
        cout << "\n======== BOOKING DETAILS ========" << endl;
        cout << "Booking ID: " << bookingId << endl;
        cout << "Passenger ID: " << passengerId << endl;
        cout << "Flight Number: " << flightNumber << endl;
        cout << "Seat Number: " << seatNumber << endl;
        cout << "Booking Date: " << bookingDate << endl;
        cout << "Total Fare: ₹" << fixed << setprecision(2) << totalFare << endl;
        cout << "Status: " << status << endl;
        cout << "=================================" << endl;
    }
    
    string getBookingId() const { return bookingId; }
    string getPassengerId() const { return passengerId; }
    string getFlightNumber() const { return flightNumber; }
    string getSeatNumber() const { return seatNumber; }
    string getStatus() const { return status; }
    double getTotalFare() const { return totalFare; }
    
    void setStatus(string s) { status = s; }
    
    string toFileString() const {
        return bookingId + "|" + passengerId + "|" + flightNumber + "|" +
               seatNumber + "|" + bookingDate + "|" + to_string(totalFare) + "|" + status;
    }
    
    static Booking fromFileString(const string& line) {
        size_t pos = 0, prev = 0;
        vector<string> tokens;
        
        while ((pos = line.find('|', prev)) != string::npos) {
            tokens.push_back(line.substr(prev, pos - prev));
            prev = pos + 1;
        }
        tokens.push_back(line.substr(prev));
        
        if (tokens.size() < 7) return Booking();
        
        Booking b(tokens[0], tokens[1], tokens[2], tokens[3], stod(tokens[5]));
        b.bookingDate = tokens[4];
        b.status = tokens[6];
        
        return b;
    }
};


class ReservationSystem {
private:
    vector<Passenger> passengers;
    vector<Admin> admins;
    vector<Flight> flights;
    vector<Booking> bookings;
    
    Passenger* currentPassenger;
    Admin* currentAdmin;
    
    const string PASSENGERS_FILE = "passengers.txt";
    const string ADMINS_FILE = "admins.txt";
    const string FLIGHTS_FILE = "flights.txt";
    const string BOOKINGS_FILE = "bookings.txt";

public:
    ReservationSystem() : currentPassenger(nullptr), currentAdmin(nullptr) {
        loadAllData();
        initializeSampleData();
    }
    
    ~ReservationSystem() {
        saveAllData();
    }
    
    void initializeSampleData() {

        if (admins.empty()) {
            admins.push_back(Admin("admin", "admin123", "System Admin", "admin@airline.com", "SUPER"));
            saveAdmins();
        }

        if (flights.empty()) {
            flights.push_back(Flight("AI101", "Air India", "New Delhi", "Mumbai", 
                                   "15/10/2025", "08:00", "10:30", 48, 5500.00));
            flights.push_back(Flight("AI102", "Air India", "Mumbai", "Bangalore", 
                                   "15/10/2025", "11:00", "13:30", 48, 4200.00));
            flights.push_back(Flight("SG201", "SpiceJet", "Bangalore", "Chennai", 
                                   "16/10/2025", "14:00", "15:30", 42, 3800.00));
            flights.push_back(Flight("IG301", "IndiGo", "Chennai", "Kolkata", 
                                   "16/10/2025", "17:00", "19:30", 54, 6500.00));
            flights.push_back(Flight("AI103", "Air India", "Delhi", "Goa", 
                                   "17/10/2025", "07:00", "09:30", 36, 7200.00));
            saveFlights();
        }
    }
    

    void saveAllData() {
        savePassengers();
        saveAdmins();
        saveFlights();
        saveBookings();
    }
    
    void loadAllData() {
        loadPassengers();
        loadAdmins();
        loadFlights();
        loadBookings();
    }
    
    void savePassengers() {
        ofstream file(PASSENGERS_FILE);
        if (file.is_open()) {
            for (const auto& p : passengers) {
                file << p.toFileString() << endl;
            }
            file.close();
        }
    }
    
    void loadPassengers() {
        ifstream file(PASSENGERS_FILE);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    passengers.push_back(Passenger::fromFileString(line));
                }
            }
            file.close();
        }
    }
    
    void saveAdmins() {
        ofstream file(ADMINS_FILE);
        if (file.is_open()) {
            for (const auto& a : admins) {
                file << a.toFileString() << endl;
            }
            file.close();
        }
    }
    
    void loadAdmins() {
        ifstream file(ADMINS_FILE);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    admins.push_back(Admin::fromFileString(line));
                }
            }
            file.close();
        }
    }
    
    void saveFlights() {
        ofstream file(FLIGHTS_FILE);
        if (file.is_open()) {
            for (const auto& f : flights) {
                file << f.toFileString() << endl;
            }
            file.close();
        }
    }
    
    void loadFlights() {
        ifstream file(FLIGHTS_FILE);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    flights.push_back(Flight::fromFileString(line));
                }
            }
            file.close();
        }
    }
    
    void saveBookings() {
        ofstream file(BOOKINGS_FILE);
        if (file.is_open()) {
            for (const auto& b : bookings) {
                file << b.toFileString() << endl;
            }
            file.close();
        }
    }
    
    void loadBookings() {
        ifstream file(BOOKINGS_FILE);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    bookings.push_back(Booking::fromFileString(line));
                }
            }
            file.close();
        }
    }
    

    void registerPassenger() {
        clearScreen();
        cout << "\n=====================================" << endl;
        cout << "     PASSENGER REGISTRATION         " << endl;
        cout << "=====================================" << endl;
        
        string name, email, phone, passport, password;
        
        cin.ignore();
        cout << "\nEnter Full Name: ";
        getline(cin, name);
        
        cout << "Enter Email: ";
        getline(cin, email);
        while (!isValidEmail(email)) {
            cout << "Invalid email format! Enter again: ";
            getline(cin, email);
        }
        
        cout << "Enter Phone: ";
        getline(cin, phone);
        while (!isValidPhone(phone)) {
            cout << "Invalid phone number! Enter 10 digits: ";
            getline(cin, phone);
        }
        
        cout << "Enter Passport Number: ";
        getline(cin, passport);
        
        cout << "Enter Password: ";
        getline(cin, password);
        while (password.length() < 4) {
            cout << "Password too short! Minimum 4 characters: ";
            getline(cin, password);
        }
        
        for (const auto& p : passengers) {
            if (p.getEmail() == email) {
                cout << "\nERROR: Email already registered!" << endl;
                pauseScreen();
                return;
            }
        }
        
        string userId = generateId("P");
        Passenger newPassenger(userId, password, name, email, phone, passport);
        passengers.push_back(newPassenger);
        savePassengers();
        
        cout << "\nSUCCESS: Registration Successful!" << endl;
        cout << "Your User ID: " << userId << endl;
        cout << "Please remember your credentials." << endl;
        pauseScreen();
    }
    
    void registerAdmin() {
        clearScreen();
        cout << "\n=====================================" << endl;
        cout << "        ADMIN REGISTRATION          " << endl;
        cout << "=====================================" << endl;
        
        string name, email, password, level;
        
        cin.ignore();
        cout << "\nEnter Full Name: ";
        getline(cin, name);
        
        cout << "Enter Email: ";
        getline(cin, email);
        while (!isValidEmail(email)) {
            cout << "Invalid email format! Enter again: ";
            getline(cin, email);
        }
        
        cout << "Enter Password: ";
        getline(cin, password);
        while (password.length() < 4) {
            cout << "Password too short! Minimum 4 characters: ";
            getline(cin, password);
        }
        
        cout << "Enter Admin Level (SUPER/REGULAR): ";
        getline(cin, level);
        level = toUpper(level);
        
        for (const auto& a : admins) {
            if (a.getEmail() == email) {
                cout << "\nERROR: Email already registered!" << endl;
                pauseScreen();
                return;
            }
        }
        
        string userId = generateId("ADM");
        Admin newAdmin(userId, password, name, email, level);
        admins.push_back(newAdmin);
        saveAdmins();
        
        cout << "\nSUCCESS: Admin Registration Successful!" << endl;
        cout << "Admin ID: " << userId << endl;
        pauseScreen();
    }
    
    bool loginPassenger() {
        clearScreen();
        cout << "\n======================================" << endl;
        cout << "        PASSENGER LOGIN             " << endl;
        cout << "=======================================" << endl;
        
        string userId, password;
        cin.ignore();
        cout << "\nUser ID: ";
        getline(cin, userId);
        cout << "Password: ";
        getline(cin, password);
        
        for (auto& p : passengers) {
            if (p.getUserId() == userId && p.getPassword() == password) {
                currentPassenger = &p;
                cout << "\nSUCCESS: Login Successful! Welcome, " << p.getName() << endl;
                pauseScreen();
                return true;
            }
        }
        
        cout << "\nERROR: Invalid credentials!" << endl;
        pauseScreen();
        return false;
    }
    
    bool loginAdmin() {
        clearScreen();
        cout << "\n======================================" << endl;
        cout << "          ADMIN LOGIN               " << endl;
        cout << "=======================================" << endl;
        
        string userId, password;
        cin.ignore();
        cout << "\nUser ID: ";
        getline(cin, userId);
        cout << "Password: ";
        getline(cin, password);
        
        for (auto& a : admins) {
            if (a.getUserId() == userId && a.getPassword() == password) {
                currentAdmin = &a;
                cout << "\nSUCCESS: Admin Login Successful! Welcome, " << a.getName() << endl;
                pauseScreen();
                return true;
            }
        }
        
        cout << "\nERROR: Invalid admin credentials!" << endl;
        pauseScreen();
        return false;
    }
    
 
    void searchFlights() {
        clearScreen();
        cout << "\n======================================" << endl;
        cout << "        SEARCH FLIGHTS              " << endl;
        cout << "=======================================" << endl;
        
        string origin, destination, date;
        cin.ignore();
        cout << "\nOrigin City: ";
        getline(cin, origin);
        cout << "Destination City: ";
        getline(cin, destination);
        cout << "Date (DD/MM/YYYY): ";
        getline(cin, date);
        
        bool found = false;
        cout << "\n*** SEARCH RESULTS ***\n";
        
        for (const auto& f : flights) {
            if (f.getOrigin() == origin && f.getDestination() == destination && 
                f.getDepartureDate() == date && f.getAvailableSeatsCount() > 0) {
                f.displayFlightInfo();
                found = true;
            }
        }
        
        if (!found) {
            cout << "\nNo flights found matching your criteria." << endl;
        }
        
        pauseScreen();
    }
    
    void viewAllFlights() {
        clearScreen();
        cout << "\n======================================" << endl;
        cout << "       AVAILABLE FLIGHTS            " << endl;
        cout << "===================================== " << endl;
        
        if (flights.empty()) {
            cout << "\nNo flights available." << endl;
            pauseScreen();
            return;
        }
        
        for (size_t i = 0; i < flights.size(); i++) {
            cout << "\n" << (i + 1) << ". ";
            flights[i].displayFlightInfo();
        }
        
        pauseScreen();
    }
    
    void bookFlight() {
        if (!currentPassenger) return;
        
        clearScreen();
        viewAllFlights();
        
        if (flights.empty()) {
            pauseScreen();
            return;
        }
        
        cout << "\nEnter Flight Number to book: ";
        string flightNum;
        cin >> flightNum;
        
        Flight* selectedFlight = nullptr;
        for (auto& f : flights) {
            if (f.getFlightNumber() == flightNum) {
                selectedFlight = &f;
                break;
            }
        }
        
        if (!selectedFlight) {
            cout << "\nERROR: Flight not found!" << endl;
            pauseScreen();
            return;
        }
        
        if (selectedFlight->getAvailableSeatsCount() <= 0) {
            cout << "\nERROR: No seats available!" << endl;
            pauseScreen();
            return;
        }
        
        selectedFlight->displaySeatMap();
        
        vector<string> availableSeats = selectedFlight->getAvailableSeats();
        cout << "\nAvailable Seats: ";
        for (size_t i = 0; i < availableSeats.size(); i++) {
            cout << availableSeats[i];
            if (i < availableSeats.size() - 1) cout << ", ";
        }
        cout << endl;
        
        string seatNum;
        cout << "\nEnter Seat Number (e.g., A1): ";
        cin >> seatNum;
        
        if (!seatNum.empty()) {
            seatNum[0] = toupper(seatNum[0]);
        }
        
        if (!selectedFlight->isSeatAvailable(seatNum)) {
            cout << "\nERROR: Seat not available!" << endl;
            pauseScreen();
            return;
        }
        
        string bookingId = generateId("TXN");
        double fare = selectedFlight->getBaseFare();
        
        Booking newBooking(bookingId, currentPassenger->getUserId(), 
                          flightNum, seatNum, fare);
        
        if (selectedFlight->bookSeat(seatNum)) {
            bookings.push_back(newBooking);
            currentPassenger->addBooking(bookingId);
            
            saveBookings();
            saveFlights();
            savePassengers();
            
            cout << "\n*** BOOKING SUCCESSFUL! ***" << endl;
            newBooking.displayBookingInfo();
        } else {
            cout << "\nERROR: Booking failed!" << endl;
        }
        
        pauseScreen();
    }
    
    void viewMyBookings() {
        if (!currentPassenger) return;
        
        clearScreen();
        cout << "\n======================================" << endl;
        cout << "        MY BOOKINGS                 " << endl;
        cout << "======================================" << endl;
        
        vector<string> myBookings = currentPassenger->getBookingIds();
        
        if (myBookings.empty()) {
            cout << "\nNo bookings found." << endl;
            pauseScreen();
            return;
        }
        
        bool found = false;
        for (const string& bId : myBookings) {
            for (const auto& b : bookings) {
                if (b.getBookingId() == bId) {
                    b.displayBookingInfo();
                    found = true;
                }
            }
        }
        
        if (!found) {
            cout << "\nNo active bookings found." << endl;
        }
        
        pauseScreen();
    }
    
    void cancelBooking() {
        if (!currentPassenger) return;
        
        clearScreen();
        viewMyBookings();
        
        string bookingId;
        cout << "\nEnter Booking ID to cancel: ";
        cin >> bookingId;
        
        for (auto& b : bookings) {
            if (b.getBookingId() == bookingId && 
                b.getPassengerId() == currentPassenger->getUserId() &&
                b.getStatus() == "CONFIRMED") {
                
                for (auto& f : flights) {
                    if (f.getFlightNumber() == b.getFlightNumber()) {
                        f.cancelSeat(b.getSeatNumber());
                        break;
                    }
                }
                
                b.setStatus("CANCELLED");
                currentPassenger->removeBooking(bookingId);
                
                saveBookings();
                saveFlights();
                savePassengers();
                
                cout << "\nSUCCESS: Booking Cancelled Successfully!" << endl;
                cout << "Refund of ₹" << fixed << setprecision(2) 
                     << b.getTotalFare() << " will be processed." << endl;
                pauseScreen();
                return;
            }
        }
        
        cout << "\nERROR: Booking not found or already cancelled!" << endl;
        pauseScreen();
    }
    
 
    void addFlight() {
        clearScreen();
        cout << "\n=====================================" << endl;
        cout << "          ADD NEW FLIGHT            " << endl;
        cout << "=====================================" << endl;
        
        string flightNum, airline, origin, dest, date, depTime, arrTime;
        int seats;
        double fare;
        
        cin.ignore();
        cout << "\nFlight Number: ";
        getline(cin, flightNum);
        cout << "Airline Name: ";
        getline(cin, airline);
        cout << "Origin: ";
        getline(cin, origin);
        cout << "Destination: ";
        getline(cin, dest);
        cout << "Date (DD/MM/YYYY): ";
        getline(cin, date);
        cout << "Departure Time (HH:MM): ";
        getline(cin, depTime);
        cout << "Arrival Time (HH:MM): ";
        getline(cin, arrTime);
        cout << "Total Seats: ";
        cin >> seats;
        cout << "Base Fare (₹): ";
        cin >> fare;
        
        for (const auto& f : flights) {
            if (f.getFlightNumber() == flightNum) {
                cout << "\nERROR: Flight number already exists!" << endl;
                pauseScreen();
                return;
            }
        }
        
        Flight newFlight(flightNum, airline, origin, dest, date, depTime, arrTime, seats, fare);
        flights.push_back(newFlight);
        saveFlights();
        
        cout << "\nSUCCESS: Flight Added Successfully!" << endl;
        pauseScreen();
    }
    
    void removeFlight() {
        clearScreen();
        viewAllFlights();
        
        if (flights.empty()) {
            pauseScreen();
            return;
        }
        
        string flightNum;
        cout << "\nEnter Flight Number to remove: ";
        cin >> flightNum;
        
        for (size_t i = 0; i < flights.size(); i++) {
            if (flights[i].getFlightNumber() == flightNum) {
                bool hasBookings = false;
                for (const auto& b : bookings) {
                    if (b.getFlightNumber() == flightNum && b.getStatus() == "CONFIRMED") {
                        hasBookings = true;
                        break;
                    }
                }
                
                if (hasBookings) {
                    cout << "\nERROR: Cannot remove flight with active bookings!" << endl;
                } else {
                    flights.erase(flights.begin() + i);
                    saveFlights();
                    cout << "\nSUCCESS: Flight Removed Successfully!" << endl;
                }
                pauseScreen();
                return;
            }
        }
        
        cout << "\nERROR: Flight not found!" << endl;
        pauseScreen();
    }
    
    void viewAllBookings() {
        clearScreen();
        cout << "\n======================================" << endl;
        cout << "      ALL BOOKINGS (ADMIN)          " << endl;
        cout << "======================================" << endl;
        
        if (bookings.empty()) {
            cout << "\nNo bookings found." << endl;
            pauseScreen();
            return;
        }
        
        int confirmed = 0, cancelled = 0;
        double totalRevenue = 0.0;
        
        for (const auto& b : bookings) {
            b.displayBookingInfo();
            if (b.getStatus() == "CONFIRMED") {
                confirmed++;
                totalRevenue += b.getTotalFare();
            } else {
                cancelled++;
            }
        }
        
        cout << "\n========== SUMMARY ==========" << endl;
        cout << "Total Bookings: " << bookings.size() << endl;
        cout << "Confirmed: " << confirmed << endl;
        cout << "Cancelled: " << cancelled << endl;
        cout << "Total Revenue: ₹" << fixed << setprecision(2) << totalRevenue << endl;
        cout << "=============================" << endl;
        
        pauseScreen();
    }
    
    void viewAllPassengers() {
        clearScreen();
        cout << "\n=====================================" << endl;
        cout << "     ALL PASSENGERS (ADMIN)         " << endl;
        cout << "======================================" << endl;
        
        if (passengers.empty()) {
            cout << "\nNo passengers registered." << endl;
            pauseScreen();
            return;
        }
        
        for (size_t i = 0; i < passengers.size(); i++) {
            cout << "\n--- Passenger " << (i + 1) << " ---" << endl;
            passengers[i].displayInfo();
        }
        
        cout << "\n\nTotal Passengers: " << passengers.size() << endl;
        pauseScreen();
    }
    
    void viewFlightOccupancy() {
        clearScreen();
        cout << "\n=====================================" << endl;
        cout << "      FLIGHT OCCUPANCY REPORT       " << endl;
        cout << "=====================================" << endl;

        if (flights.empty()) {
            cout << "\nNo flights available." << endl;
            pauseScreen();
            return;
        }

        for (const auto& f : flights) {
            int booked = f.getTotalSeats() - f.getAvailableSeatsCount();
            double occupancy = f.getTotalSeats() > 0 ? (booked * 100.0) / f.getTotalSeats() : 0.0;
            
            cout << "\n----------------------------------------" << endl;
            cout << "Flight: " << f.getFlightNumber() << endl;
            cout << "Route: " << f.getOrigin() << " -> " << f.getDestination() << endl;
            cout << "Total Seats: " << f.getTotalSeats() << endl;
            cout << "Booked: " << booked << endl;
            cout << "Available: " << f.getAvailableSeatsCount() << endl;
            cout << "Occupancy: " << fixed << setprecision(2) << occupancy << "%" << endl;
            
            cout << "Status: [";
            int filled = (int)(occupancy / 5);
            for (int i = 0; i < 20; i++) {
                if (i < filled) cout << "#";
                else cout << "-";
            }
            cout << "] " << occupancy << "%" << endl;
        }
        
        pauseScreen();
    }
    
    void generateReports() {
        clearScreen();
        cout << "\n=====================================" << endl;
        cout << "        SYSTEM REPORTS              " << endl;
        cout << "======================================" << endl;
        
        int totalFlights = flights.size();
        int totalPassengers = passengers.size();
        int totalBookings = bookings.size();
        int confirmedBookings = 0;
        int cancelledBookings = 0;
        double totalRevenue = 0.0;
        int totalSeats = 0;
        int bookedSeats = 0;
        
        for (const auto& f : flights) {
            totalSeats += f.getTotalSeats();
            bookedSeats += (f.getTotalSeats() - f.getAvailableSeatsCount());
        }
        
        for (const auto& b : bookings) {
            if (b.getStatus() == "CONFIRMED") {
                confirmedBookings++;
                totalRevenue += b.getTotalFare();
            } else {
                cancelledBookings++;
            }
        }
        
        double avgBookingValue = confirmedBookings > 0 ? totalRevenue / confirmedBookings : 0.0;
        double systemOccupancy = totalSeats > 0 ? (bookedSeats * 100.0) / totalSeats : 0.0;
        
        cout << "\n========== AIRLINE SYSTEM REPORT ==========" << endl;
        cout << "\n--- FLEET STATISTICS ---" << endl;
        cout << "Total Flights: " << totalFlights << endl;
        cout << "Total Seat Capacity: " << totalSeats << endl;
        cout << "Booked Seats: " << bookedSeats << endl;
        cout << "Available Seats: " << (totalSeats - bookedSeats) << endl;
        cout << "Overall Occupancy: " << fixed << setprecision(2) << systemOccupancy << "%" << endl;
        
        cout << "\n--- PASSENGER STATISTICS ---" << endl;
        cout << "Registered Passengers: " << totalPassengers << endl;
        cout << "Total Bookings: " << totalBookings << endl;
        cout << "Confirmed Bookings: " << confirmedBookings << endl;
        cout << "Cancelled Bookings: " << cancelledBookings << endl;
        
        cout << "\n--- FINANCIAL STATISTICS ---" << endl;
        cout << "Total Revenue: ₹" << fixed << setprecision(2) << totalRevenue << endl;
        cout << "Average Booking Value: ₹" << fixed << setprecision(2) << avgBookingValue << endl;
        
        cout << "\n--- TOP ROUTES ---" << endl;
        map<string, int> routeFrequency;
        for (const auto& b : bookings) {
            if (b.getStatus() == "CONFIRMED") {
                for (const auto& f : flights) {
                    if (f.getFlightNumber() == b.getFlightNumber()) {
                        string route = f.getOrigin() + " -> " + f.getDestination();
                        routeFrequency[route]++;
                        break;
                    }
                }
            }
        }
        
        if (routeFrequency.empty()) {
            cout << "No booking data available." << endl;
        } else {
            vector<pair<string, int>> routes(routeFrequency.begin(), routeFrequency.end());
            sort(routes.begin(), routes.end(), 
                 [](const pair<string, int>& a, const pair<string, int>& b) {
                     return a.second > b.second;
                 });
            
            int rank = 1;
            for (const auto& route : routes) {
                cout << rank++ << ". " << route.first << " (" << route.second << " bookings)" << endl;
                if (rank > 5) break;
            }
        }
        
        cout << "\n===========================================" << endl;
        cout << "Report Generated on: " << getCurrentDate() << endl;
        cout << "===========================================" << endl;
        
        pauseScreen();
    }
    

    void passengerMenu() {
        int choice;
        do {
            clearScreen();
            cout << "\n=====================================" << endl;
            cout << "      PASSENGER DASHBOARD           " << endl;
            cout << "=====================================" << endl;
            cout << "\nWelcome, " << currentPassenger->getName() << "!" << endl;
            cout << "\n1. Search Flights" << endl;
            cout << "2. View All Flights" << endl;
            cout << "3. Book a Flight" << endl;
            cout << "4. View My Bookings" << endl;
            cout << "5. Cancel Booking" << endl;
            cout << "6. My Profile" << endl;
            cout << "7. Logout" << endl;
            cout << "----------------------------------" << endl;
            cout << "Enter choice: ";
            cin >> choice;
            
            switch (choice) {
                case 1:
                    searchFlights();
                    break;
                case 2:
                    viewAllFlights();
                    break;
                case 3:
                    bookFlight();
                    break;
                case 4:
                    viewMyBookings();
                    break;
                case 5:
                    cancelBooking();
                    break;
                case 6:
                    clearScreen();
                    cout << "\n=== MY PROFILE ===" << endl;
                    currentPassenger->displayInfo();
                    pauseScreen();
                    break;
                case 7:
                    currentPassenger = nullptr;
                    cout << "\nLogged out successfully!" << endl;
                    pauseScreen();
                    break;
                default:
                    cout << "\nInvalid choice!" << endl;
                    pauseScreen();
            }
        } while (choice != 7);
    }
    
    void adminMenu() {
        int choice;
        do {
            clearScreen();
            cout << "\n=====================================" << endl;
            cout << "       ADMIN DASHBOARD              " << endl;
            cout << "=====================================" << endl;
            cout << "\nWelcome, " << currentAdmin->getName() << "!" << endl;
            cout << "\n1. Add New Flight" << endl;
            cout << "2. Remove Flight" << endl;
            cout << "3. View All Flights" << endl;
            cout << "4. View All Bookings" << endl;
            cout << "5. View All Passengers" << endl;
            cout << "6. Flight Occupancy Report" << endl;
            cout << "7. Generate System Reports" << endl;
            cout << "8. Register New Admin" << endl;
            cout << "9. Logout" << endl;
            cout << "-------------------------------------" << endl;
            cout << "Enter choice: ";
            cin >> choice;
            
            switch (choice) {
                case 1:
                    addFlight();
                    break;
                case 2:
                    removeFlight();
                    break;
                case 3:
                    viewAllFlights();
                    break;
                case 4:
                    viewAllBookings();
                    break;
                case 5:
                    viewAllPassengers();
                    break;
                case 6:
                    viewFlightOccupancy();
                    break;
                case 7:
                    generateReports();
                    break;
                case 8:
                    if (currentAdmin->getAdminLevel() == "SUPER") {
                        registerAdmin();
                    } else {
                        cout << "\nERROR: Only SUPER admins can register new admins!" << endl;
                        pauseScreen();
                    }
                    break;
                case 9:
                    currentAdmin = nullptr;
                    cout << "\nLogged out successfully!" << endl;
                    pauseScreen();
                    break;
                default:
                    cout << "\nInvalid choice!" << endl;
                    pauseScreen();
            }
        } while (choice != 9);
    }
    
    void mainMenu() {
        int choice;
        do {
            clearScreen();
            cout << "\n=====================================" << endl;
            cout << "   AIRLINE RESERVATION SYSTEM       " << endl;
            cout << "         (Pro Level Edition)        " << endl;
            cout << "=====================================" << endl;
            cout << "\n1. Passenger Login" << endl;
            cout << "2. Passenger Registration" << endl;
            cout << "3. Admin Login" << endl;
            cout << "4. Exit" << endl;
            cout << "-------------------------------------" << endl;
            cout << "Enter choice: ";
            cin >> choice;
            
            switch (choice) {
                case 1:
                    if (loginPassenger()) {
                        passengerMenu();
                    }
                    break;
                case 2:
                    registerPassenger();
                    break;
                case 3:
                    if (loginAdmin()) {
                        adminMenu();
                    }
                    break;
                case 4:
                    saveAllData();
                    cout << "\nThank you for using Airline Reservation System!" << endl;
                    cout << "Safe travels!\n" << endl;
                    break;
                default:
                    cout << "\nInvalid choice! Please try again." << endl;
                    pauseScreen();
            }
        } while (choice != 4);
    }
    
    void run() {
        mainMenu();
    }
};

 
int main() {
    try {
        ReservationSystem system;
        system.run();
    } catch (const exception& e) {
        cerr << "\nERROR: System Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}