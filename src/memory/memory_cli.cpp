#include <iostream>
#include <vector>
#include <limits>
#include "memory.hpp"

using namespace std;

void runMemoryCLI() {
    vector<MemoryBlock> memory;
    int totalInternalFragmentation = 0;

    // Initial memory setup (can be customized)
    memory.push_back(MemoryBlock(-1, 300, false));

    while (true) {
        cout << "\n====== Memory Manager CLI ======\n";
        cout << "1. Allocate Memory\n";
        cout << "2. Free Memory\n";
        cout << "3. Print Memory State\n";
        cout << "4. Exit to Main Menu\n";
        cout << "Choose an option: ";

        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); // Clear input error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << " Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            int id, size;
            cout << "Enter Process ID: ";
            cin >> id;
            cout << "Enter Memory Size to Allocate (KB): ";
            cin >> size;

            bool success = allocateMemory(memory, id, size, totalInternalFragmentation);
            if (!success) {
                int external = calculateExternalFragmentation(memory, size);
                if (external > 0) {
                    cout << " External Fragmentation Detected: " << external << "KB\n";
                }
            }
        }

        else if (choice == 2) {
            int id;
            cout << "Enter Process ID to Free: ";
            cin >> id;
            freeMemory(memory, id);
        }

        else if (choice == 3) {
            printMemoryState(memory);
            cout << "Total Internal Fragmentation: " << totalInternalFragmentation << "KB\n";
        }

        else if (choice == 4) {
            cout << " Returning to Main Menu...\n";
            break;
        }

        else {
            cout << " Invalid option. Please select from 1 to 4.\n";
        }
    }
}
