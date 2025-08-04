#include <iostream>
#include <vector>
#include <limits>
#include "memory.hpp"

using namespace std;

void runMemoryCLI() {
    vector<MemoryBlock> memory;
    int totalInternalFragmentation = 0;
    int n;

    cout << "\n--- Memory Manager CLI ---\n";
    cout << "Enter number of memory blocks: " << flush;
    cin >> n;

    for (int i = 0; i < n; ++i) {
        int size;
        cout << "Enter size (in KB) for Block " << i + 1 << ": ";
        cin >> size;
        memory.push_back(MemoryBlock(-1, size, false));
    }

    int choice;
    while (true) {
        cout << "\nMemory Operations:\n";
        cout << " 1. Allocate Memory\n";
        cout << " 2. Free Memory\n";
        cout << " 3. Show Memory State\n";
        cout << " 4. Show Fragmentation\n";
        cout << " 5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            int id, size;
            cout << "Enter Process ID: ";
            cin >> id;
            cout << "Enter Size to Allocate (KB): ";
            cin >> size;
            allocateMemory(memory, id, size, totalInternalFragmentation);
        } else if (choice == 2) {
            int id;
            cout << "Enter Process ID to Free: ";
            cin >> id;
            freeMemory(memory, id);
        } else if (choice == 3) {
            printMemoryState(memory);
        } else if (choice == 4) {
            int reqSize;
            cout << "Enter size to check fragmentation (KB): ";
            cin >> reqSize;
            int external = calculateExternalFragmentation(memory, reqSize);
            cout << "External Fragmentation: " << external << "KB\n";
            cout << "Internal Fragmentation: " << totalInternalFragmentation << "KB\n";
        } else if (choice == 5) {
            cout << "Exiting Memory Manager...\n";
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
}

