#include "../virtual_memory/virtual_memory.hpp"
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

void runVirtualMemoryCLI()
{
    int memorySize, pageSize;
    cout << "=== Virtual Memory Manager ===\n";
    cout << "Enter total memory size (e.g., 64): ";
    cin >> memorySize;
    cout << "Enter page size (e.g., 4): ";
    cin >> pageSize;

    int policyChoice;
    cout << "\nSelect page replacement policy:\n";
    cout << "1. FIFO (First-In, First-Out)\n";
    cout << "2. LRU (Least Recently Used)\n";
    cout << "3. Clock (Second-Chance)\n";
    cout << "4. Optimal (Run a predefined benchmark simulation)\n";
    cout << "Choice: ";
    cin >> policyChoice;

    ReplacementPolicy policy;
    switch (policyChoice) {
        case 1:
            policy = ReplacementPolicy::FIFO;
            break;
        case 2:
            policy = ReplacementPolicy::LRU;
            break;
        case 3:
            policy = ReplacementPolicy::CLOCK;
            break;
        case 4:
            policy = ReplacementPolicy::OPTIMAL;
            break;
        default:
            cout << "Invalid policy choice. Exiting.\n";
            return;
    }
    // Special handling for the Optimal policy
    if (policy == ReplacementPolicy::OPTIMAL) {
        cout << "\n--- Running Predefined Optimal Simulation --- \n";
        VirtualMemoryManager vmm(memorySize, pageSize, policy);

        vmm.allocateProcess(1); // Process 1 has pages 0-4
        vmm.allocateProcess(2); // Process 2 has pages 0-3

        // Format: {processId, virtualPageNumber}
        vector<pair<int, int>> referenceString = {
            {1, 0}, {2, 0}, {1, 1}, {2, 1}, {1, 2}, {2, 2},
            {1, 3}, {1, 4}, {2, 3}, {1, 1}, {2, 0}, {1, 2}
        };

        cout << "Using reference string: ";
        for(const auto& p : referenceString) {
            cout << "P" << p.first << ":" << p.second << " ";
        }
        cout << "\n";

        
        vmm.runOptimalSimulation(referenceString);
        
        cout << "\nOptimal simulation finished.\n";
        return; // Exit after the one-shot simulation
    }

    // instance for interactive policies
    VirtualMemoryManager vmm(memorySize, pageSize, policy);

    cout << "\nSelect Log Level for this session:\n";
    cout << "1. Normal (default)\n";
    cout << "2. Verbose (show steps)\n";
    cout << "3. Debug (show addresses and extra detail)\n";
    cout << "Choice: ";
    int logLevelChoice;
    cin >> logLevelChoice;
    
    switch(logLevelChoice) {
        case 2:
            vmm.setLogLevel(VERBOSE);
            break;
        case 3:
            vmm.setLogLevel(DEBUG);
            break;
        default:
            vmm.setLogLevel(NORMAL);
            break;
    }


    cout << "\nStarting interactive session...\n";

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
            int pid;
            cout << "Enter Process ID: ";
            cin >> pid;
            vmm.allocateProcess(pid);
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
