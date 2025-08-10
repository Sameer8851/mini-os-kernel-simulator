#include "../virtual_memory/virtual_memory.hpp"
#include <iostream>

using namespace std;

void runVirtualMemoryCLI()
{
    int memorySize, pageSize;
    cout << "=== Virtual Memory Manager ===\n";
    cout << "Enter total memory size (in KB): ";
    cin >> memorySize;
    cout << "Enter page size (in KB): ";
    cin >> pageSize;

    int policyChoice;
    cout << "Select page replacement policy:\n1. FIFO\n2. LRU\nChoice: ";
    cin >> policyChoice;

    ReplacementPolicy policy = (policyChoice == 2) ? ReplacementPolicy::LRU : ReplacementPolicy::FIFO;

    VirtualMemoryManager vmm(memorySize, pageSize, policy);

    while (true)
    {
        cout << "\n===== Virtual Memory Manager Menu =====\n";
        cout << "1. Allocate Process\n";
        cout << "2. Access Page\n";
        cout << "3. Free Process\n";
        cout << "4. Print Page Table\n";
        cout << "5. Print Frame Table\n";
        cout << "6. Total page faults\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int pid, numPages;
            cout << "Enter Process ID: ";
            cin >> pid;
            cout << "Enter number of pages to allocate: ";
            cin >> numPages;
            vmm.allocateProcess(pid, numPages);
            break;
        }

        case 2:
        {
            int pid, vpn;
            cout << "Enter Process ID: ";
            cin >> pid;
            cout << "Enter Virtual Page Number to Access: ";
            cin >> vpn;
            vmm.accessPage(pid, vpn);
            break;
        }

        case 3:
        {
            int pid;
            cout << "Enter Process ID to free: ";
            cin >> pid;
            vmm.freeProcess(pid);
            break;
        }

        case 4:
            vmm.printPageTable();
            break;

        case 5:
            vmm.printFrameTable();
            break;

        case 6:
            cout << "Total page faults so far: " << vmm.getPageFaults() << "\n";
            break;

        case 7:
            cout << "Exiting Virtual Memory Manager CLI...\n";
            return;

        default:
            cout << "Invalid choice. Try again.\n";
        }
    }
}
