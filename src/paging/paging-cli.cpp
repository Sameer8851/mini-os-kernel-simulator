#include "paging.hpp"
#include <iostream>

using namespace std;

void runPagingCLI()
{
    int memSize, frameSize;
    cout << "Enter total memory size (KB): ";
    cin >> memSize;
    cout << "Enter frame size (KB): ";
    cin >> frameSize;

    PagingManager pm(memSize, frameSize);

    int choice;
    while (true)
    {
        cout << "\n===== Paging Manager Menu =====\n";
        cout << "1. Allocate Process\n";
        cout << "2. Free Process\n";
        cout << "3. Show Page Table\n";
        cout << "4. Show Frame Table\n";
        cout << "5. Translate Logical Address\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int pid, size;
            cout << "Enter Process ID: ";
            cin >> pid;
            cout << "Enter Process Size (KB): ";
            cin >> size;
            pm.allocateProcess(pid, size);
        }
        else if (choice == 2)
        {
            int pid;
            cout << "Enter Process ID to free: ";
            cin >> pid;
            pm.freeProcess(pid);
        }
        else if (choice == 3)
        {
            pm.printPageTable();
        }
        else if (choice == 4)
        {
            pm.printFrameTable();
        }
        else if (choice == 5)
        {
            int pid, logicalAddr;
            cout << "Enter Process ID: ";
            cin >> pid;
            cout << "Enter Logical Address (in KB): ";
            cin >> logicalAddr;
            pm.translateAddress(pid, logicalAddr);
        }
        else if (choice == 6)
        {
            cout << "Exiting Paging Manager...\n";
            break;
        }
        else
        {
            cout << "Invalid choice. Try again.\n";
        }
    }
}
