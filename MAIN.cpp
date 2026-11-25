// ============================================================
//  EcoSort - AI-Based Intelligent Waste Management System
//  main.cpp : joins Part 1 (Member 1) and Part 2 (Member 2)
//  Compile:  g++ -std=c++11 main.cpp -o ecosort
// ============================================================
#include <iostream>
#include "common.h"
#include "part1_requests.h"   // Member 1: Hash Table, Queue, Heap
#include "part2_routing.h"    // Member 2: Circular List, Graph
using namespace std;

int main() {
    RequestManager requests;   // Part 1
    RoutePlanner   routes;     // Part 2
    int ch;
    do {
        cout << "\n===== EcoSort : Intelligent Waste Management =====\n"
            << "1. Classify a waste item            [Part 1]\n"
            << "2. Add collection request           [Part 1]\n"
            << "3. Process queue -> priority heap   [Part 1]\n"
            << "4. Dispatch truck & plan route      [Part 1 + 2]\n"
            << "5. Show pending requests/trucks     [Part 1 + 2]\n"
            << "6. Show city map                    [Part 2]\n"
            << "0. Exit\nChoice: ";
        if (!(cin >> ch)) break;
        cin.ignore(1000, '\n');

        if (ch == 1) {
            string item; cout << "Enter waste item: "; getline(cin, item);
            requests.classifyOnly(item);
        }
        else if (ch == 2) {
            string item; int loc;
            cout << "Waste item: "; getline(cin, item);
            routes.showMap();
            cout << "Pickup location number (1-" << routes.locationCount() - 1 << "): ";
            if (!(cin >> loc)) { cin.clear(); cin.ignore(1000, '\n'); cout << "Invalid.\n"; continue; }
            cin.ignore(1000, '\n');
            if (!routes.validLocation(loc)) { cout << "Invalid location.\n"; continue; }
            requests.addRequest(item, loc);
        }
        else if (ch == 3) {
            requests.processQueue();
        }
        else if (ch == 4) {
            if (!requests.hasScheduled()) { cout << " No scheduled requests. (Use option 3 first.)\n"; continue; }
            Request batch[4];
            int n = requests.popUrgent(batch, 4);   // Part 1 gives urgent requests
            routes.dispatch(batch, n);              // Part 2 assigns truck + route
        }
        else if (ch == 5) {
            requests.showPending();
            routes.showTrucks();
        }
        else if (ch == 6) {
            routes.showMap();
        }
        else if (ch == 0) {
            cout << "Goodbye!\n";
        }
        else cout << "Invalid choice.\n";
    } while (ch != 0);
    return 0;
}