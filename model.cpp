#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

class Room {
public:
    int roomNumber;
    string roomType;
    bool isBooked;
    double pricePerNight;

    Room(int number, string type, double price) 
        : roomNumber(number), roomType(type), isBooked(false), pricePerNight(price) {}
};

class Guest {
public:
    string name;
    string contactNumber;
    string idProof;
    int roomNumber;
    time_t checkInDate;
    time_t checkOutDate;

    Guest(string n, string contact, string id, int room, time_t in, time_t out)
        : name(n), contactNumber(contact), idProof(id), roomNumber(room), checkInDate(in), checkOutDate(out) {}
};

class HotelManagementSystem {
private:
    vector<Room> rooms;
    map<int, Guest*> roomToGuestMap;  
    map<string, Guest*> guestRecords; 
    queue<int> availableRooms;        

    void initializeRooms() {
        for (int i = 101; i <= 110; ++i) {
            rooms.push_back(Room(i, "Standard", 100.0));
            availableRooms.push(i);
        }
        for (int i = 201; i <= 205; ++i) {
            rooms.push_back(Room(i, "Deluxe", 200.0));
            availableRooms.push(i);
        }
        for (int i = 301; i <= 303; ++i) {
            rooms.push_back(Room(i, "Suite", 500.0));
            availableRooms.push(i);
        }
    }

    Room* findRoom(int roomNumber) {
        int left = 0, right = rooms.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (rooms[mid].roomNumber == roomNumber) {
                return &rooms[mid];
            } else if (rooms[mid].roomNumber < roomNumber) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return nullptr;
    }

public:
    HotelManagementSystem() {
        initializeRooms();
    }

    bool bookRoom(string name, string contact, string idProof, string roomType, int nights) {
        if (guestRecords.find(idProof) != guestRecords.end()) {
            cout << "Guest with this ID is already registered.\n";
            return false;
        }

        queue<int> tempQueue = availableRooms;
        int selectedRoom = -1;

        while (!tempQueue.empty()) {
            int roomNum = tempQueue.front();
            tempQueue.pop();
            Room* room = findRoom(roomNum);
            if (room && !room->isBooked && room->roomType == roomType) {
                selectedRoom = roomNum;
                break;
            }
        }

        if (selectedRoom == -1) {
            cout << "No available " << roomType << " rooms.\n";
            return false;
        }

        time_t now = time(0);
        time_t checkout = now + (nights * 24 * 60 * 60); 

        Guest* newGuest = new Guest(name, contact, idProof, selectedRoom, now, checkout);
        
        roomToGuestMap[selectedRoom] = newGuest;
        guestRecords[idProof] = newGuest;
        findRoom(selectedRoom)->isBooked = true;

        queue<int> newQueue;
        while (!availableRooms.empty()) {
            int room = availableRooms.front();
            availableRooms.pop();
            if (room != selectedRoom) {
                newQueue.push(room);
            }
        }
        availableRooms = newQueue;

        cout << "Room " << selectedRoom << " booked successfully for " << name << ".\n";
        return true;
    }

    bool checkOut(int roomNumber) {
        if (roomToGuestMap.find(roomNumber) == roomToGuestMap.end()) {
            cout << "Room " << roomNumber << " is not currently booked.\n";
            return false;
        }

        Guest* guest = roomToGuestMap[roomNumber];
        
        Room* room = findRoom(roomNumber);
        double nights = difftime(guest->checkOutDate, guest->checkInDate) / (24 * 60 * 60);
        double totalAmount = nights * room->pricePerNight;

        cout << "\n=== BILL ===\n";
        cout << "Guest Name: " << guest->name << "\n";
        cout << "Room Number: " << roomNumber << "\n";
        cout << "Room Type: " << room->roomType << "\n";
        cout << "Check-in: " << ctime(&guest->checkInDate);
        cout << "Check-out: " << ctime(&guest->checkOutDate);
        cout << "Total Nights: " << nights << "\n";
        cout << "Rate per night: $" << room->pricePerNight << "\n";
        cout << "Total Amount: $" << totalAmount << "\n";
        cout << "============\n";

        room->isBooked = false;
        guestRecords.erase(guest->idProof);
        roomToGuestMap.erase(roomNumber);
        availableRooms.push(roomNumber);
        
        delete guest;
        cout << "Check-out completed successfully.\n";
        return true;
    }

    void displayRoomStatus() {
        cout << "\n=== ROOM STATUS ===\n";
        cout << left << setw(10) << "Room No." << setw(15) << "Type" 
             << setw(10) << "Price/Night" << setw(10) << "Status" << "\n";
        
        for (const Room& room : rooms) {
            cout << left << setw(10) << room.roomNumber 
                 << setw(15) << room.roomType
                 << setw(10) << room.pricePerNight
                 << setw(10) << (room.isBooked ? "Booked" : "Available") << "\n";
        }
    }

    void searchGuest(string idProof) {
        if (guestRecords.find(idProof) == guestRecords.end()) {
            cout << "Guest not found.\n";
            return;
        }

        Guest* guest = guestRecords[idProof];
        Room* room = findRoom(guest->roomNumber);

        cout << "\n=== GUEST DETAILS ===\n";
        cout << "Name: " << guest->name << "\n";
        cout << "Contact: " << guest->contactNumber << "\n";
        cout << "ID Proof: " << guest->idProof << "\n";
        cout << "Room Number: " << guest->roomNumber << "\n";
        cout << "Room Type: " << room->roomType << "\n";
        cout << "Check-in Date: " << ctime(&guest->checkInDate);
        cout << "Check-out Date: " << ctime(&guest->checkOutDate);
    }

    void displayAvailableRooms(string roomType) {
        cout << "\n=== AVAILABLE " << roomType << " ROOMS ===\n";
        bool found = false;
        
        queue<int> tempQueue = availableRooms;
        while (!tempQueue.empty()) {
            int roomNum = tempQueue.front();
            tempQueue.pop();
            Room* room = findRoom(roomNum);
            if (room && room->roomType == roomType) {
                cout << "Room " << roomNum << " - $" << room->pricePerNight << "/night\n";
                found = true;
            }
        }
        
        if (!found) {
            cout << "No available " << roomType << " rooms.\n";
        }
    }
};

int main() {
    HotelManagementSystem hotel;
    int choice;
    
    do {
        cout << "\n=== HOTEL MANAGEMENT SYSTEM ===\n";
        cout << "1. Book a Room\n";
        cout << "2. Check Out\n";
        cout << "3. View Room Status\n";
        cout << "4. Search Guest\n";
        cout << "5. View Available Rooms by Type\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); 

        switch (choice) {
            case 1: {
                string name, contact, id, roomType;
                int nights;
                
                cout << "Enter guest name: ";
                getline(cin, name);
                cout << "Enter contact number: ";
                getline(cin, contact);
                cout << "Enter ID proof: ";
                getline(cin, id);
                cout << "Enter room type (Standard/Deluxe/Suite): ";
                getline(cin, roomType);
                cout << "Enter number of nights: ";
                cin >> nights;
                
                hotel.bookRoom(name, contact, id, roomType, nights);
                break;
            }
            case 2: {
                int roomNum;
                cout << "Enter room number to check out: ";
                cin >> roomNum;
                hotel.checkOut(roomNum);
                break;
            }
            case 3:
                hotel.displayRoomStatus();
                break;
            case 4: {
                string id;
                cout << "Enter guest ID proof to search: ";
                getline(cin, id);
                hotel.searchGuest(id);
                break;
            }
            case 5: {
                string roomType;
                cout << "Enter room type to view available rooms (Standard/Deluxe/Suite): ";
                getline(cin, roomType);
                hotel.displayAvailableRooms(roomType);
                break;
            }
            case 6:
                cout << "Exiting system...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 6);

    return 0;
}
