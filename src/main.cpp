#include <iostream>
#include "scheduler/scheduler.hpp"
#include "memory/memory.hpp"
#include "paging/paging.hpp"
#include "memory/virtual_memory/virtual_memory.hpp"

using namespace std;

void bootSystem() {
    cout << "Initializing Mini OS Kernel Simulator (MOSKS)..." << endl;
    cout << "--> Loading CPU Scheduler..." << endl;
    cout << "--> Loading Memory Manager..." << endl;
    cout << "--> Mounting File System..." << endl;
    cout << "--> Launching Shell Interface..." << endl;
    cout << "System Boot Complete [OK]" << endl;
}

int main() {
    bootSystem();

    int choice;

    while (true) {
    cout << "\n====== MOSKS Main Menu ======\n";
    cout << "1. Run CPU Scheduler\n";
    cout << "2. Run Memory Manager\n";
    cout << "3. Run File System (Coming Soon)\n";
    cout << "4. Run Paging Simulation\n";
    cout << "5. Run Virtual Memory Simulation \n"; 
    cout << "6. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            runScheduler();
            break;
        case 2:
            runMemoryCLI();
            break;
        case 3:
            cout << "File System module not implemented yet.\n";
            break;
        case 4:
            runPagingCLI();
            break;
        case 5:
            runVirtualMemoryCLI();
            break;
        case 6:
            cout << "Shutting down MOSKS...\n";
            return 0;
        default:
            cout << "Invalid choice. Try again.\n";
    }
}

    return 0;
}
